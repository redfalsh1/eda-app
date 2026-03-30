#ifndef UI_PROJECT_TREE_H
#define UI_PROJECT_TREE_H

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QToolBar>
#include <QLineEdit>

/**
 * @brief 项目文件树控件
 * 
 * 显示项目文件结构：
 * - 库（Library）
 * - 单元（Cell）
 * - 视图（View：layout/schematic/symbol）
 * 
 * 支持：
 * - 树形结构显示
 * - 右键菜单
 * - 拖拽操作
 * - 搜索过滤
 */
class ProjectTree : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectTree(QWidget *parent = nullptr);
    ~ProjectTree();

    // 项目操作
    void loadProject(const QString &projectPath);
    void closeProject();
    QString currentProject() const { return m_projectPath; }
    
    // 选中项
    QString selectedCell() const;
    QString selectedView() const;

signals:
    void fileOpened(const QString &file);
    void cellSelected(const QString &cellName);
    void viewOpened(const QString &viewName);
    void projectLoaded(const QString &projectPath);

private slots:
    void onItemDoubleClicked(const QModelIndex &index);
    void onSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void onFilterTextChanged(const QString &text);
    void onNewCell();
    void onRenameCell();
    void onDeleteCell();
    void onRefresh();

private:
    void setupUI();
    void setupConnections();
    void populateTree();
    void createContextMenu();

    QToolBar *m_toolBar;
    QTreeView *m_treeView;
    QStandardItemModel *m_model;
    QLineEdit *m_filterEdit;
    
    QString m_projectPath;
    QMenu *m_contextMenu;
};

#endif // UI_PROJECT_TREE_H
