#include "ui/layout_canvas.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QRubberBand>
#include <QPainter>
#include <QScrollBar>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

LayoutCanvas::LayoutCanvas(QWidget *parent)
    : QGraphicsView(parent)
    , m_zoomLevel(1.0)
    , m_minZoom(0.001)
    , m_maxZoom(100.0)
    , m_gridVisible(true)
    , m_gridSize(20)
    , m_panning(false)
    , m_drawing(false)
    , m_drawTool(ShapeType::Rectangle)
    , m_drawPreview(nullptr)
    , m_currentLayerId(1)
    , m_nextShapeId(1)
{
    setupScene();
    setupViewport();
    setupHighDPI();
    
    m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

LayoutCanvas::~LayoutCanvas()
{
}

void LayoutCanvas::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-50000, -50000, 100000, 100000);  // 100mm x 100mm
    m_scene->setBackgroundBrush(QBrush(QColor("#1e1e1e")));
    setScene(m_scene);
}

void LayoutCanvas::setupViewport()
{
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    setInteractive(true);
    setMouseTracking(true);
    
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
}

void LayoutCanvas::setupHighDPI()
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

// ============ 缩放控制 ============

void LayoutCanvas::zoomIn()
{
    applyZoom(1.2, viewport()->rect().center());
}

void LayoutCanvas::zoomOut()
{
    applyZoom(0.8, viewport()->rect().center());
}

void LayoutCanvas::zoomToFit()
{
    QRectF rect = m_scene->itemsBoundingRect();
    if (rect.isEmpty()) {
        rect = QRectF(-1000, -1000, 2000, 2000);
    }
    fitInView(rect, Qt::KeepAspectRatio);
    m_zoomLevel = transform().m11();
    emit zoomLevelChanged(m_zoomLevel);
}

void LayoutCanvas::zoomTo(double factor)
{
    double currentZoom = transform().m11();
    double targetZoom = factor;
    
    if (targetZoom < m_minZoom) targetZoom = m_minZoom;
    if (targetZoom > m_maxZoom) targetZoom = m_maxZoom;
    
    double zoomFactor = targetZoom / currentZoom;
    applyZoom(zoomFactor, viewport()->rect().center());
}

void LayoutCanvas::applyZoom(double factor, const QPointF &center)
{
    double newZoom = m_zoomLevel * factor;
    
    if (newZoom < m_minZoom || newZoom > m_maxZoom) {
        return;
    }
    
    scale(factor, factor);
    m_zoomLevel = newZoom;
    
    emit zoomLevelChanged(m_zoomLevel);
}

// ============ 网格控制 ============

void LayoutCanvas::toggleGrid()
{
    m_gridVisible = !m_gridVisible;
    m_scene->invalidate();
}

void LayoutCanvas::setGridVisible(bool visible)
{
    if (m_gridVisible != visible) {
        m_gridVisible = visible;
        m_scene->invalidate();
    }
}

void LayoutCanvas::setGridSize(int size)
{
    if (size > 0) {
        m_gridSize = size;
        m_scene->invalidate();
    }
}

// ============ 图层控制 ============

void LayoutCanvas::setCurrentLayer(int layerId)
{
    m_currentLayerId = layerId;
}

// ============ 选择操作 ============

void LayoutCanvas::selectAll()
{
    for (auto it = m_shapes.begin(); it != m_shapes.end(); ++it) {
        it->selected = true;
        if (it->graphicsItem) {
            it->graphicsItem->setSelected(true);
        }
    }
    updateSelection();
}

void LayoutCanvas::clearSelection()
{
    for (auto it = m_shapes.begin(); it != m_shapes.end(); ++it) {
        it->selected = false;
        if (it->graphicsItem) {
            it->graphicsItem->setSelected(false);
        }
    }
    updateSelection();
}

QVector<LayoutShape*> LayoutCanvas::selectedShapes() const
{
    QVector<LayoutShape*> selected;
    for (auto it = m_shapes.begin(); it != m_shapes.end(); ++it) {
        if (it->selected) {
            selected.append(&it.value());
        }
    }
    return selected;
}

// ============ 编辑操作 ============

void LayoutCanvas::cut()
{
    copy();
    deleteSelected();
}

void LayoutCanvas::copy()
{
    m_clipboard.clear();
    for (const auto &shape : m_shapes) {
        if (shape.selected) {
            m_clipboard.append(shape);
        }
    }
    
    // 复制到系统剪贴板
    QClipboard *clipboard = QApplication::clipboard();
    QMimeData *mimeData = new QMimeData();
    // TODO: 序列化图形数据
    clipboard->setMimeData(mimeData);
}

void LayoutCanvas::paste()
{
    for (const auto &shape : m_clipboard) {
        LayoutShape newShape = shape;
        newShape.id = generateShapeId();
        // 偏移位置
        for (auto &point : newShape.points) {
            point += QPointF(10, 10);
        }
        m_shapes[newShape.id] = newShape;
        // TODO: 创建图形项
        emit shapeAdded(newShape.id);
    }
    updateSelection();
}

void LayoutCanvas::deleteSelected()
{
    QList<int> toRemove;
    for (auto it = m_shapes.begin(); it != m_shapes.end(); ++it) {
        if (it->selected) {
            toRemove.append(it.key());
            if (it->graphicsItem) {
                m_scene->removeItem(it->graphicsItem);
                delete it->graphicsItem;
            }
        }
    }
    
    for (int id : toRemove) {
        m_shapes.remove(id);
        emit shapeRemoved(id);
    }
    
    updateSelection();
}

// ============ 画布操作 ============

void LayoutCanvas::clear()
{
    m_scene->clear();
    m_shapes.clear();
    m_nextShapeId = 1;
    m_zoomLevel = 1.0;
    resetTransform();
    emit selectionChanged(0);
}

void LayoutCanvas::resetView()
{
    resetTransform();
    centerOn(0, 0);
    m_zoomLevel = 1.0;
    emit zoomLevelChanged(m_zoomLevel);
}

int LayoutCanvas::addRectangle(const QRectF &rect)
{
    LayoutShape shape;
    shape.id = generateShapeId();
    shape.type = ShapeType::Rectangle;
    shape.layerId = m_currentLayerId;
    shape.points = {
        rect.topLeft(),
        rect.topRight(),
        rect.bottomRight(),
        rect.bottomLeft()
    };
    
    QGraphicsRectItem *item = new QGraphicsRectItem(rect);
    item->setBrush(QBrush(QColor(0, 122, 204, 100)));
    item->setPen(QPen(QColor(0, 122, 204), 1));
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    
    shape.graphicsItem = item;
    m_shapes[shape.id] = shape;
    m_scene->addItem(item);
    
    emit shapeAdded(shape.id);
    return shape.id;
}

int LayoutCanvas::addPolygon(const QVector<QPointF> &points)
{
    LayoutShape shape;
    shape.id = generateShapeId();
    shape.type = ShapeType::Polygon;
    shape.layerId = m_currentLayerId;
    shape.points = points;
    
    QPolygonF polygon(points);
    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(polygon);
    item->setBrush(QBrush(QColor(0, 122, 204, 100)));
    item->setPen(QPen(QColor(0, 122, 204), 1));
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    
    shape.graphicsItem = item;
    m_shapes[shape.id] = shape;
    m_scene->addItem(item);
    
    emit shapeAdded(shape.id);
    return shape.id;
}

// ============ 事件处理 ============

void LayoutCanvas::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::NoModifier) {
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;
        
        double zoomFactor = 1.0;
        if (!numPixels.isNull()) {
            zoomFactor = 1.0 + (numPixels.y() / 120.0) * 0.1;
        } else if (!numDegrees.isNull()) {
            zoomFactor = 1.0 + (numDegrees.y() / 120.0) * 0.1;
        }
        
        if (zoomFactor != 1.0) {
            applyZoom(zoomFactor, event->pos());
        }
        
        event->accept();
        return;
    }
    
    QGraphicsView::wheelEvent(event);
}

void LayoutCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else if (event->button() == Qt::LeftButton) {
        if (event->modifiers() == Qt::ControlModifier) {
            // Ctrl+ 左键：框选
            m_selectionStart = event->pos();
            m_rubberBand->setGeometry(QRect(m_selectionStart, QSize()));
            m_rubberBand->show();
        } else if (event->modifiers() == Qt::ShiftModifier) {
            // Shift+ 左键：绘制模式
            m_drawing = true;
            m_drawStart = event->pos();
        } else {
            // 普通点击：选择
            QGraphicsItem *item = itemAt(event->pos());
            if (item) {
                if (event->modifiers() == Qt::ShiftModifier) {
                    item->setSelected(!item->isSelected());
                } else {
                    clearSelection();
                    item->setSelected(true);
                }
            } else {
                clearSelection();
            }
        }
        event->accept();
    }
    
    QGraphicsView::mousePressEvent(event);
}

void LayoutCanvas::mouseMoveEvent(QMouseEvent *event)
{
    updateCursorPos(event->pos());
    
    if (m_panning) {
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();
        
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        
        event->accept();
        return;
    }
    
    if (m_drawing && m_drawPreview) {
        // 更新绘制预览
        QRectF rect = QRectF(mapToScene(m_drawStart), mapToScene(event->pos())).normalized();
        QGraphicsRectItem *rectItem = qgraphicsitem_cast<QGraphicsRectItem*>(m_drawPreview);
        if (rectItem) {
            rectItem->setRect(rect);
        }
        event->accept();
        return;
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void LayoutCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else if (event->button() == Qt::LeftButton && m_drawing) {
        // 完成绘制
        m_drawing = false;
        QRectF rect = QRectF(mapToScene(m_drawStart), mapToScene(event->pos())).normalized();
        if (rect.width() > 1 && rect.height() > 1) {
            addRectangle(rect);
        }
        if (m_drawPreview) {
            m_scene->removeItem(m_drawPreview);
            delete m_drawPreview;
            m_drawPreview = nullptr;
        }
        event->accept();
        return;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

void LayoutCanvas::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            deleteSelected();
            event->accept();
            break;
        case Qt::Key_Escape:
            clearSelection();
            m_drawing = false;
            event->accept();
            break;
        case Qt::Key_G:
            toggleGrid();
            event->accept();
            break;
        default:
            QGraphicsView::keyPressEvent(event);
            break;
    }
}

void LayoutCanvas::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->fillRect(rect, QColor("#1e1e1e"));
    
    if (m_gridVisible) {
        drawGrid(painter, rect);
    }
}

void LayoutCanvas::updateCursorPos(const QPoint &pos)
{
    QPointF scenePos = mapToScene(pos);
    emit cursorPositionChanged(scenePos);
}

void LayoutCanvas::drawGrid(QPainter *painter, const QRectF &rect)
{
    painter->save();
    
    QPen pen(QColor("#3e3e42"));
    pen.setWidthF(0.5);
    painter->setPen(pen);
    
    double gridStep = m_gridSize / m_zoomLevel;
    double left = qFloor(rect.left() / gridStep) * gridStep;
    double top = qFloor(rect.top() / gridStep) * gridStep;
    double right = rect.right();
    double bottom = rect.bottom();
    
    for (double x = left; x <= right; x += gridStep) {
        painter->drawLine(QLineF(x, rect.top(), x, rect.bottom()));
    }
    
    for (double y = top; y <= bottom; y += gridStep) {
        painter->drawLine(QLineF(rect.left(), y, rect.right(), y));
    }
    
    painter->restore();
}

void LayoutCanvas::updateSelection()
{
    int count = 0;
    for (const auto &shape : m_shapes) {
        if (shape.selected) {
            count++;
        }
    }
    emit selectionChanged(count);
}

int LayoutCanvas::generateShapeId()
{
    return m_nextShapeId++;
}
