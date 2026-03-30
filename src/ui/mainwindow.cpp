#include "ui/mainwindow.h"
#include "ui/layout_canvas.h"
#include "ui/project_tree.h"
#include "ui/layer_panel.h"
#include "ui/property_editor.h"
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentFile("")
{
    setupUI();
    setupConnections();
    loadStyleSheet();
    
    setWindowTitle(tr("EDA 版图编辑器 - 未命名"));
    resize(1400, 900);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();
}

void MainWindow::setupMenuBar()
{
    m_menuBar = menuBar();
    
    // === 文件菜单 ===
    m_fileMenu = m_menuBar->addMenu(tr("文件 (&F)"));
    m_fileMenu->addAction(tr("新建 (&N)"), this, &MainWindow::onNew, QKeySequence::New);
    m_fileMenu->addAction(tr("打开 (&O)..."), this, &MainWindow::onOpen, QKeySequence::Open);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("保存 (&S)"), this, &MainWindow::onSave, QKeySequence::Save);
    m_fileMenu->addAction(tr("另存为 (&A)..."), this, &MainWindow::onSaveAs, QKeySequence::SaveAs);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("导入..."), this, &MainWindow::onImport);
    m_fileMenu->addAction(tr("导出..."), this, &MainWindow::onExport);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(tr("退出 (&X)"), this, &MainWindow::onExit, QKeySequence::Quit);
    
    // === 编辑菜单 ===
    m_editMenu = m_menuBar->addMenu(tr("编辑 (&E)"));
    m_undoAction = m_editMenu->addAction(tr("撤销 (&U)"), this, &MainWindow::onUndo, QKeySequence::Undo);
    m_redoAction = m_editMenu->addAction(tr("重做 (&R)"), this, &MainWindow::onRedo, QKeySequence::Redo);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("剪切 (&T)"), this, &MainWindow::onCut, QKeySequence::Cut);
    m_editMenu->addAction(tr("复制 (&C)"), this, &MainWindow::onCopy, QKeySequence::Copy);
    m_editMenu->addAction(tr("粘贴 (&P)"), this, &MainWindow::onPaste, QKeySequence::Paste);
    m_editMenu->addAction(tr("删除 (&D)"), this, &MainWindow::onDelete, QKeySequence::Delete);
    m_editMenu->addSeparator();
    m_editMenu->addAction(tr("全选 (&A)"), this, &MainWindow::onSelectAll, QKeySequence::SelectAll);
    
    // === 视图菜单 ===
    m_viewMenu = m_menuBar->addMenu(tr("视图 (&V)"));
    m_viewMenu->addAction(tr("放大 (&I)"), this, &MainWindow::onZoomIn, QKeySequence::ZoomIn);
    m_viewMenu->addAction(tr("缩小 (&O)"), this, &MainWindow::onZoomOut, QKeySequence::ZoomOut);
    m_viewMenu->addAction(tr("适应窗口 (&F)"), this, &MainWindow::onZoomFit);
    m_viewMenu->addAction(tr("100% (&1)"), this, &MainWindow::onZoom100);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(tr("显示网格 (&G)"), this, &MainWindow::onToggleGrid);
    m_viewMenu->addAction(tr("显示标尺 (&R)"), this, &MainWindow::onToggleRulers);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(tr("重置布局"), this, &MainWindow::onResetLayout);
    
    // === 工具菜单 ===
    m_toolsMenu = m_menuBar->addMenu(tr("工具 (&T)"));
    m_toolsMenu->addAction(tr("DRC 检查 (&D)..."), this, &MainWindow::onDRC);
    m_toolsMenu->addSeparator();
    m_toolsMenu->addAction(tr("属性编辑器 (&P)"), this, &MainWindow::onProperties);
    m_toolsMenu->addSeparator();
    m_toolsMenu->addAction(tr("设置 (&S)..."), this, &MainWindow::onSettings);
    
    // === 窗口菜单 ===
    m_windowMenu = m_menuBar->addMenu(tr("窗口 (&W)"));
    m_windowMenu->addAction(tr("项目文件树"), this, &MainWindow::onToggleProjectDock);
    m_windowMenu->addAction(tr("图层管理"), this, &MainWindow::onToggleLayerDock);
    m_windowMenu->addAction(tr("属性编辑器"), this, &MainWindow::onTogglePropertyDock);
    
    // === 帮助菜单 ===
    m_helpMenu = m_menuBar->addMenu(tr("帮助 (&H)"));
    m_helpMenu->addAction(tr("帮助文档 (&H)"), this, &MainWindow::onHelp, QKeySequence::HelpContents);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(tr("关于 EDA (&A)"), this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    setIconSize(QSize(20, 20));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    // === 文件工具栏 ===
    m_fileToolBar = addToolBar(tr("文件"));
    m_fileToolBar->setObjectName("fileToolBar");
    m_fileToolBar->addAction(tr("新建"), this, &MainWindow::onNew);
    m_fileToolBar->addAction(tr("打开"), this, &MainWindow::onOpen);
    m_fileToolBar->addAction(tr("保存"), this, &MainWindow::onSave);
    m_fileToolBar->addSeparator();
    m_fileToolBar->addAction(tr("导入"), this, &MainWindow::onImport);
    
    // === 编辑工具栏 ===
    m_editToolBar = addToolBar(tr("编辑"));
    m_editToolBar->setObjectName("editToolBar");
    m_editToolBar->addAction(m_undoAction);
    m_editToolBar->addAction(m_redoAction);
    m_editToolBar->addSeparator();
    m_editToolBar->addAction(tr("剪切"), this, &MainWindow::onCut);
    m_editToolBar->addAction(tr("复制"), this, &MainWindow::onCopy);
    m_editToolBar->addAction(tr("粘贴"), this, &MainWindow::onPaste);
    m_editToolBar->addAction(tr("删除"), this, &MainWindow::onDelete);
    
    // === 视图工具栏 ===
    m_viewToolBar = addToolBar(tr("视图"));
    m_viewToolBar->setObjectName("viewToolBar");
    m_viewToolBar->addAction(tr("放大"), this, &MainWindow::onZoomIn);
    m_viewToolBar->addAction(tr("缩小"), this, &MainWindow::onZoomOut);
    m_viewToolBar->addAction(tr("适应"), this, &MainWindow::onZoomFit);
    m_viewToolBar->addAction(tr("100%"), this, &MainWindow::onZoom100);
    m_viewToolBar->addSeparator();
    m_viewToolBar->addAction(tr("网格"), this, &MainWindow::onToggleGrid);
    
    // === 工具工具栏 ===
    m_toolsToolBar = addToolBar(tr("工具"));
    m_toolsToolBar->setObjectName("toolsToolBar");
    m_toolsToolBar->addAction(tr("DRC"), this, &MainWindow::onDRC);
    m_toolsToolBar->addAction(tr("属性"), this, &MainWindow::onProperties);
    m_toolsToolBar->addAction(tr("设置"), this, &MainWindow::onSettings);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    
    // 坐标显示
    m_cursorLabel = new QLabel(tr("坐标：(0.00, 0.00) μm"), this);
    m_cursorLabel->setMinimumWidth(180);
    m_statusBar->addPermanentWidget(m_cursorLabel, 0);
    
    // 缩放显示
    m_zoomLabel = new QLabel(tr("缩放：100%"), this);
    m_zoomLabel->setMinimumWidth(100);
    m_statusBar->addPermanentWidget(m_zoomLabel, 0);
    
    // 选中数量
    m_selectionLabel = new QLabel(tr("选中：0"), this);
    m_selectionLabel->setMinimumWidth(80);
    m_statusBar->addPermanentWidget(m_selectionLabel, 0);
    
    // 消息显示
    m_messageLabel = new QLabel(tr("就绪"), this);
    m_messageLabel->setMinimumWidth(200);
    m_statusBar->addWidget(m_messageLabel, 1);
}

void MainWindow::setupDockWidgets()
{
    // === 左侧：项目文件树 ===
    m_projectTree = new ProjectTree(this);
    m_projectDock = new QDockWidget(tr("项目文件树"), this);
    m_projectDock->setObjectName("projectDock");
    m_projectDock->setWidget(m_projectTree);
    m_projectDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_projectDock);
    
    // === 右侧：图层管理面板 ===
    m_layerPanel = new LayerPanel(this);
    m_layerDock = new QDockWidget(tr("图层管理"), this);
    m_layerDock->setObjectName("layerDock");
    m_layerDock->setWidget(m_layerPanel);
    m_layerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_layerDock);
    
    // === 右侧：属性编辑器（停靠在图层下方）===
    m_propertyEditor = new PropertyEditor(this);
    m_propertyDock = new QDockWidget(tr("属性编辑器"), this);
    m_propertyDock->setObjectName("propertyDock");
    m_propertyDock->setWidget(m_propertyEditor);
    m_propertyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    
    // 设置 tab 化
    tabifyDockWidget(m_layerDock, m_propertyDock);
    m_layerDock->raise();
    
    // === 中央：版图绘制画布 ===
    m_canvas = new LayoutCanvas(this);
    setCentralWidget(m_canvas);
}

void MainWindow::setupConnections()
{
    // 画布信号
    connect(m_canvas, &LayoutCanvas::cursorPositionChanged,
            this, &MainWindow::updateCursorPosition);
    
    connect(m_canvas, &LayoutCanvas::zoomLevelChanged,
            this, &MainWindow::updateZoomLevel);
    
    connect(m_canvas, &LayoutCanvas::selectionChanged,
            this, &MainWindow::updateSelectionCount);
    
    // 项目树信号
    connect(m_projectTree, &ProjectTree::fileOpened,
            this, [this](const QString &file) {
                m_currentFile = file;
                setWindowTitle(tr("EDA 版图编辑器 - %1").arg(QFileInfo(file).fileName()));
            });
    
    // 图层面板信号
    connect(m_layerPanel, &LayerPanel::currentLayerChanged,
            m_canvas, &LayoutCanvas::setCurrentLayer);
}

void MainWindow::loadStyleSheet()
{
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QString::fromUtf8(styleFile.readAll());
        qApp->setStyleSheet(styleSheet);
        styleFile.close();
    }
}

// ============ 槽函数实现 ============

void MainWindow::onNew()
{
    m_currentFile.clear();
    setWindowTitle(tr("EDA 版图编辑器 - 未命名"));
    m_canvas->clear();
    m_statusBar->showMessage(tr("新建文件"), 2000);
}

void MainWindow::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("打开版图文件"),
        QString(),
        tr("版图文件 (*.lay *.gds);;GDSII 文件 (*.gds *.gdsii);;所有文件 (*)")
    );
    
    if (!fileName.isEmpty()) {
        m_currentFile = fileName;
        setWindowTitle(tr("EDA 版图编辑器 - %1").arg(QFileInfo(fileName).fileName()));
        m_statusBar->showMessage(tr("打开文件：%1").arg(fileName), 3000);
        // TODO: 加载文件
    }
}

void MainWindow::onSave()
{
    if (m_currentFile.isEmpty()) {
        onSaveAs();
    } else {
        // TODO: 实现保存逻辑
        m_statusBar->showMessage(tr("保存文件：%1").arg(m_currentFile), 2000);
    }
}

void MainWindow::onSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("另存为"),
        QString(),
        tr("版图文件 (*.lay);;GDSII 文件 (*.gds);;所有文件 (*)")
    );
    
    if (!fileName.isEmpty()) {
        m_currentFile = fileName;
        setWindowTitle(tr("EDA 版图编辑器 - %1").arg(QFileInfo(fileName).fileName()));
        m_statusBar->showMessage(tr("文件已保存"), 2000);
    }
}

void MainWindow::onImport()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("导入文件"),
        QString(),
        tr("GDSII 文件 (*.gds *.gdsii);;所有文件 (*)")
    );
    
    if (!fileName.isEmpty()) {
        // TODO: 导入 GDSII
        m_statusBar->showMessage(tr("导入文件：%1").arg(fileName), 3000);
    }
}

void MainWindow::onExport()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("导出文件"),
        QString(),
        tr("GDSII 文件 (*.gds);;SVG 文件 (*.svg);;PDF 文件 (*.pdf)")
    );
    
    if (!fileName.isEmpty()) {
        // TODO: 导出
        m_statusBar->showMessage(tr("导出文件：%1").arg(fileName), 2000);
    }
}

void MainWindow::onExit()
{
    close();
}

void MainWindow::onUndo()
{
    // TODO: 实现撤销
    m_statusBar->showMessage(tr("撤销"), 1000);
}

void MainWindow::onRedo()
{
    // TODO: 实现重做
    m_statusBar->showMessage(tr("重做"), 1000);
}

void MainWindow::onCut()
{
    m_canvas->cut();
    m_statusBar->showMessage(tr("剪切"), 1000);
}

void MainWindow::onCopy()
{
    m_canvas->copy();
    m_statusBar->showMessage(tr("复制"), 1000);
}

void MainWindow::onPaste()
{
    m_canvas->paste();
    m_statusBar->showMessage(tr("粘贴"), 1000);
}

void MainWindow::onDelete()
{
    m_canvas->deleteSelected();
    m_statusBar->showMessage(tr("删除"), 1000);
}

void MainWindow::onSelectAll()
{
    m_canvas->selectAll();
    m_statusBar->showMessage(tr("全选"), 1000);
}

void MainWindow::onZoomIn()
{
    m_canvas->zoomIn();
}

void MainWindow::onZoomOut()
{
    m_canvas->zoomOut();
}

void MainWindow::onZoomFit()
{
    m_canvas->zoomToFit();
}

void MainWindow::onZoom100()
{
    m_canvas->zoomTo(1.0);
}

void MainWindow::onToggleGrid()
{
    m_canvas->toggleGrid();
}

void MainWindow::onToggleRulers()
{
    // TODO: 实现标尺显示/隐藏
    m_statusBar->showMessage(tr("切换标尺"), 1000);
}

void MainWindow::onResetLayout()
{
    // 重置面板布局
    m_projectDock->show();
    m_layerDock->show();
    m_propertyDock->show();
    
    m_projectDock->raise();
    m_layerDock->raise();
    
    // 恢复默认位置
    addDockWidget(Qt::LeftDockWidgetArea, m_projectDock);
    addDockWidget(Qt::RightDockWidgetArea, m_layerDock);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    
    m_statusBar->showMessage(tr("布局已重置"), 2000);
}

void MainWindow::onDRC()
{
    // TODO: 运行 DRC 检查
    m_statusBar->showMessage(tr("运行 DRC 检查..."), 2000);
}

void MainWindow::onSettings()
{
    // TODO: 打开设置对话框
    m_statusBar->showMessage(tr("打开设置"), 1000);
}

void MainWindow::onProperties()
{
    m_propertyDock->show();
    m_propertyDock->raise();
}

void MainWindow::onToggleProjectDock()
{
    m_projectDock->setVisible(!m_projectDock->isVisible());
}

void MainWindow::onToggleLayerDock()
{
    m_layerDock->setVisible(!m_layerDock->isVisible());
}

void MainWindow::onTogglePropertyDock()
{
    m_propertyDock->setVisible(!m_propertyDock->isVisible());
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("关于 EDA 版图编辑器"),
        tr("<h2>EDA 版图编辑器</h2>"
           "<p>版本：1.0.0</p>"
           "<p>专业的 IC 版图设计工具</p>"
           "<p>© 2026 EDA Team</p>"
           "<p>基于 Qt5.15+ 开发</p>"));
}

void MainWindow::onHelp()
{
    // TODO: 打开帮助文档
    m_statusBar->showMessage(tr("打开帮助文档"), 1000);
}

void MainWindow::updateCursorPosition(const QPointF &pos)
{
    m_cursorLabel->setText(tr("坐标：(%1, %2) μm")
        .arg(pos.x(), 0, 'f', 2)
        .arg(pos.y(), 0, 'f', 2));
}

void MainWindow::updateZoomLevel(double zoom)
{
    m_zoomLabel->setText(tr("缩放：%1%").arg(zoom * 100, 0, 'f', 0));
}

void MainWindow::updateSelectionCount(int count)
{
    m_selectionLabel->setText(tr("选中：%1").arg(count));
}

void MainWindow::updateMessage(const QString &msg)
{
    m_messageLabel->setText(msg);
}

void MainWindow::saveLayoutState(QSettings &settings)
{
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/state", saveState());
}

void MainWindow::restoreLayoutState(const QSettings &settings)
{
    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    restoreState(settings.value("mainWindow/state").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 保存布局状态
    QSettings settings;
    saveLayoutState(settings);
    
    // TODO: 检查未保存的更改
    QMessageBox::StandardButton ret = QMessageBox::question(
        this,
        tr("确认退出"),
        tr("确定要退出 EDA 版图编辑器吗？"),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (ret == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}
