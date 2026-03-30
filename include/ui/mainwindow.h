#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QLabel>
#include <QMdiArea>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LayoutCanvas;
class ProjectTree;
class LayerPanel;
class PropertyEditor;

/**
 * @brief EDA 版图编辑器主窗口
 * 
 * 三栏式可停靠布局：
 * - 左侧：项目文件树（QDockWidget）
 * - 中央：版图绘制画布（LayoutCanvas）
 * - 右侧：图层管理面板（QDockWidget）
 * 
 * 支持：
 * - 面板拖拽浮动
 * - 面板隐藏/显示
 * - 布局状态保存/恢复
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    /**
     * @brief 获取版图画布指针
     * @return LayoutCanvas* 画布对象
     */
    LayoutCanvas* layoutCanvas() const { return m_canvas; }

    /**
     * @brief 获取项目树指针
     * @return ProjectTree* 项目树控件
     */
    ProjectTree* projectTree() const { return m_projectTree; }

    /**
     * @brief 获取图层面板指针
     * @return LayerPanel* 图层面板
     */
    LayerPanel* layerPanel() const { return m_layerPanel; }

    /**
     * @brief 保存布局状态
     * @param settings QSettings 对象
     */
    void saveLayoutState(QSettings &settings);

    /**
     * @brief 恢复布局状态
     * @param settings QSettings 对象
     */
    void restoreLayoutState(const QSettings &settings);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件菜单
    void onNew();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onImport();
    void onExport();
    void onExit();
    
    // 编辑菜单
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onSelectAll();
    
    // 视图菜单
    void onZoomIn();
    void onZoomOut();
    void onZoomFit();
    void onZoom100();
    void onToggleGrid();
    void onToggleRulers();
    void onResetLayout();
    
    // 工具菜单
    void onDRC();
    void onSettings();
    void onProperties();
    
    // 窗口菜单
    void onToggleProjectDock();
    void onToggleLayerDock();
    void onTogglePropertyDock();
    
    // 帮助菜单
    void onAbout();
    void onHelp();
    
    // 状态栏更新
    void updateCursorPosition(const QPointF &pos);
    void updateZoomLevel(double zoom);
    void updateSelectionCount(int count);
    void updateMessage(const QString &msg);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDockWidgets();
    void setupConnections();
    void loadStyleSheet();
    void createActions();

    // 菜单栏
    QMenuBar *m_menuBar;
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_toolsMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;
    
    // 工具栏
    QToolBar *m_fileToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_viewToolBar;
    QToolBar *m_toolsToolBar;
    
    // 状态栏
    QStatusBar *m_statusBar;
    QLabel *m_cursorLabel;      // 坐标显示
    QLabel *m_zoomLabel;        // 缩放显示
    QLabel *m_selectionLabel;   // 选中数量
    QLabel *m_messageLabel;     // 消息显示
    
    // 中央画布
    LayoutCanvas *m_canvas;
    
    // 可停靠面板
    QDockWidget *m_projectDock;     // 项目文件树
    QDockWidget *m_layerDock;       // 图层管理
    QDockWidget *m_propertyDock;    // 属性编辑器
    
    ProjectTree *m_projectTree;
    LayerPanel *m_layerPanel;
    PropertyEditor *m_propertyEditor;
    
    // 当前文件
    QString m_currentFile;
    
    // 动作
    QAction *m_undoAction;
    QAction *m_redoAction;
};

#endif // UI_MAINWINDOW_H
