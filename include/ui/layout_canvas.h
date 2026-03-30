#ifndef UI_LAYOUT_CANVAS_H
#define UI_LAYOUT_CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPointF>
#include <QVector>
#include <QMap>

/**
 * @brief 版图图形类型
 */
enum class ShapeType {
    Rectangle,    ///< 矩形
    Polygon,      ///< 多边形
    Path,         ///< 路径
    Text,         ///< 文本
    Reference     ///< 引用单元
};

/**
 * @brief 版图图形数据
 */
struct LayoutShape {
    int id;
    ShapeType type;
    int layerId;
    QVector<QPointF> points;
    QColor color;
    bool selected;
    
    LayoutShape() : id(0), type(ShapeType::Rectangle), layerId(0), selected(false) {}
};

/**
 * @brief EDA 版图绘制画布
 * 
 * 基于 QGraphicsView 的高性能版图编辑器，支持：
 * - 鼠标滚轮缩放（0.1x - 100x）
 * - 中键平移，左键选择/绘制，右键菜单
 * - 网格吸附（可配置间距）
 * - 多层渲染
 * - 高 DPI 适配
 * - 剪切/复制/粘贴
 */
class LayoutCanvas : public QGraphicsView
{
    Q_OBJECT

public:
    explicit LayoutCanvas(QWidget *parent = nullptr);
    ~LayoutCanvas();

    // 缩放控制
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void zoomTo(double factor);
    double zoomLevel() const { return m_zoomLevel; }
    
    // 网格控制
    void toggleGrid();
    void setGridVisible(bool visible);
    bool isGridVisible() const { return m_gridVisible; }
    void setGridSize(int size);
    int gridSize() const { return m_gridSize; }
    
    // 图层控制
    void setCurrentLayer(int layerId);
    int currentLayer() const { return m_currentLayerId; }
    
    // 选择操作
    void selectAll();
    void clearSelection();
    QVector<LayoutShape*> selectedShapes() const;
    
    // 编辑操作
    void cut();
    void copy();
    void paste();
    void deleteSelected();
    
    // 画布操作
    void clear();
    void resetView();
    
    // 添加图形
    int addRectangle(const QRectF &rect);
    int addPolygon(const QVector<QPointF> &points);
    
    // 高 DPI 适配
    void setupHighDPI();

signals:
    void cursorPositionChanged(const QPointF &pos);
    void zoomLevelChanged(double zoom);
    void selectionChanged(int count);
    void shapeAdded(int shapeId);
    void shapeRemoved(int shapeId);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    void setupScene();
    void setupViewport();
    void updateCursorPos(const QPoint &pos);
    void applyZoom(double factor, const QPointF &center);
    void drawGrid(QPainter *painter, const QRectF &rect);
    void updateSelection();
    int generateShapeId();

    QGraphicsScene *m_scene;
    
    // 缩放状态
    double m_zoomLevel;
    double m_minZoom;
    double m_maxZoom;
    
    // 网格状态
    bool m_gridVisible;
    int m_gridSize;
    
    // 平移状态
    bool m_panning;
    QPoint m_lastPanPoint;
    
    // 绘制状态
    bool m_drawing;
    ShapeType m_drawTool;
    QPoint m_drawStart;
    QGraphicsItem *m_drawPreview;
    
    // 选择状态
    QRubberBand *m_rubberBand;
    QPoint m_selectionStart;
    
    // 图层
    int m_currentLayerId;
    
    // 图形数据
    QMap<int, LayoutShape> m_shapes;
    int m_nextShapeId;
    
    // 剪贴板
    QVector<LayoutShape> m_clipboard;
};

#endif // UI_LAYOUT_CANVAS_H
