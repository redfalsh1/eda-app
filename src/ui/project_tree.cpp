#include "ui/project_tree.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>

ProjectTree::ProjectTree(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
    
    setMinimumWidth(200);
}

ProjectTree::~ProjectTree()
{
}

void ProjectTree::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // === 工具栏 ===
    m_toolBar = new QToolBar(this);
    m_toolBar->setIconSize(QSize(16, 16));
    m_toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolBar->setMovable(false);
    m_toolBar->setFloatable(false);
    
    QAction *openAct = m_toolBar->addAction(tr("📂 打开"));
    openAct->setToolTip(tr("打开项目"));
    connect(openAct, &QAction::triggered, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("打开项目"));
        if (!dir.isEmpty()) {
            loadProject(dir);
        }
    });
    
    m_toolBar->addSeparator();
    
    QAction *newCellAct = m_toolBar->addAction(tr("➕ 新建单元"));
    newCellAct->setToolTip(tr("新建单元"));
    connect(newCellAct, &QAction::triggered, this, &ProjectTree::onNewCell);
    
    QAction *refreshAct = m_toolBar->addAction(tr("🔄 刷新"));
    refreshAct->setToolTip(tr("刷新项目树"));
    connect(refreshAct, &QAction::triggered, this, &ProjectTree::onRefresh);
    
    mainLayout->addWidget(m_toolBar);
    
    // === 过滤栏 ===
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText(tr("搜索..."));
    m_filterEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(m_filterEdit);
    
    // === 树视图 ===
    m_treeView = new QTreeView(this);
    m_treeView->setHeaderHidden(true);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setEditTriggers(QAbstractItemView::DoubleClicked);
    
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(1);
    m_treeView->setModel(m_model);
    
    mainLayout->addWidget(m_treeView);
    
    // 创建上下文菜单
    createContextMenu();
    
    // 添加示例数据
    populateTree();
}

void ProjectTree::setupConnections()
{
    connect(m_filterEdit, &QLineEdit::textChanged,
            this, &ProjectTree::onFilterTextChanged);
    
    connect(m_treeView, &QTreeView::doubleClicked,
            this, &ProjectTree::onItemDoubleClicked);
    
    connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ProjectTree::onSelectionChanged);
}

void ProjectTree::populateTree()
{
    m_model->clear();
    
    // 根节点
    QStandardItem *root = m_model->invisibleRootItem();
    
    // 示例：库
    QStandardItem *libItem = new QStandardItem("📚 tech_lib");
    libItem->setData("library", Qt::UserRole);
    
    // 示例：单元
    QStandardItem *cell1 = new QStandardItem("📦 nmos");
    cell1->setData("cell", Qt::UserRole);
    
    QStandardItem *cell1Layout = new QStandardItem("📐 layout");
    cell1Layout->setData("view_layout", Qt::UserRole);
    
    QStandardItem *cell1Schematic = new QStandardItem("📝 schematic");
    cell1Schematic->setData("view_schematic", Qt::UserRole);
    
    cell1->appendRow(cell1Layout);
    cell1->appendRow(cell1Schematic);
    
    // 示例：单元 2
    QStandardItem *cell2 = new QStandardItem("📦 pmos");
    cell2->setData("cell", Qt::UserRole);
    
    QStandardItem *cell2Layout = new QStandardItem("📐 layout");
    cell2Layout->setData("view_layout", Qt::UserRole);
    
    cell2->appendRow(cell2Layout);
    
    // 示例：单元 3
    QStandardItem *cell3 = new QStandardItem("📦 inverter");
    cell3->setData("cell", Qt::UserRole);
    
    QStandardItem *cell3Layout = new QStandardItem("📐 layout");
    cell3Layout->setData("view_layout", Qt::UserRole);
    
    QStandardItem *cell3Schematic = new QStandardItem("📝 schematic");
    cell3Schematic->setData("view_schematic", Qt::UserRole);
    
    cell3->appendRow(cell3Layout);
    cell3->appendRow(cell3Schematic);
    
    // 组装树
    libItem->appendRow(cell1);
    libItem->appendRow(cell2);
    libItem->appendRow(cell3);
    root->appendRow(libItem);
    
    // 展开
    m_treeView->expandAll();
}

void ProjectTree::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    QAction *newAct = m_contextMenu->addAction(tr("新建单元"));
    connect(newAct, &QAction::triggered, this, &ProjectTree::onNewCell);
    
    m_contextMenu->addSeparator();
    
    QAction *renameAct = m_contextMenu->addAction(tr("重命名"));
    connect(renameAct, &QAction::triggered, this, &ProjectTree::onRenameCell);
    
    QAction *deleteAct = m_contextMenu->addAction(tr("删除"));
    connect(deleteAct, &QAction::triggered, this, &ProjectTree::onDeleteCell);
    
    m_contextMenu->addSeparator();
    
    QAction *refreshAct = m_contextMenu->addAction(tr("刷新"));
    connect(refreshAct, &QAction::triggered, this, &ProjectTree::onRefresh);
    
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeView, &QTreeView::customContextMenuRequested,
            this, [this](const QPoint &pos) {
                QModelIndex index = m_treeView->indexAt(pos);
                if (index.isValid()) {
                    m_treeView->selectionModel()->setCurrentIndex(
                        index, QItemSelectionModel::Select);
                    m_contextMenu->exec(m_treeView->viewport()->mapToGlobal(pos));
                }
            });
}

void ProjectTree::loadProject(const QString &projectPath)
{
    m_projectPath = projectPath;
    populateTree();
    emit projectLoaded(projectPath);
    emit fileOpened(projectPath);
}

void ProjectTree::closeProject()
{
    m_projectPath.clear();
    m_model->clear();
}

QString ProjectTree::selectedCell() const
{
    QModelIndex index = m_treeView->currentIndex();
    if (index.isValid()) {
        QStandardItem *item = m_model->itemFromIndex(index);
        if (item && item->data(Qt::UserRole).toString() == "cell") {
            return item->text();
        }
    }
    return QString();
}

QString ProjectTree::selectedView() const
{
    QModelIndex index = m_treeView->currentIndex();
    if (index.isValid()) {
        QStandardItem *item = m_model->itemFromIndex(index);
        if (item) {
            QString type = item->data(Qt::UserRole).toString();
            if (type.startsWith("view_")) {
                return type.mid(5);
            }
        }
    }
    return QString();
}

void ProjectTree::onItemDoubleClicked(const QModelIndex &index)
{
    QStandardItem *item = m_model->itemFromIndex(index);
    if (!item) return;
    
    QString type = item->data(Qt::UserRole).toString();
    
    if (type == "view_layout") {
        emit viewOpened("layout");
        // 打开版图视图
        QString cellName = item->parent()->text();
        emit fileOpened(QString("%1/%2/layout").arg(m_projectPath).arg(cellName));
    } else if (type == "view_schematic") {
        emit viewOpened("schematic");
        // TODO: 打开原理图插件
    }
}

void ProjectTree::onSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    
    if (current.isValid()) {
        QStandardItem *item = m_model->itemFromIndex(current);
        if (item) {
            QString type = item->data(Qt::UserRole).toString();
            if (type == "cell") {
                emit cellSelected(item->text());
            }
        }
    }
}

void ProjectTree::onFilterTextChanged(const QString &text)
{
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStandardItem *item = m_model->item(row);
        if (item) {
            bool matches = item->text().contains(text, Qt::CaseInsensitive);
            m_treeView->setRowHidden(row, !matches);
        }
    }
}

void ProjectTree::onNewCell()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("新建单元"),
        tr("单元名称:"), QLineEdit::Normal, QString(), &ok);
    
    if (ok && !name.isEmpty()) {
        // TODO: 创建新单元
        QMessageBox::information(this, tr("新建单元"),
            tr("创建单元：%1").arg(name));
    }
}

void ProjectTree::onRenameCell()
{
    QModelIndex index = m_treeView->currentIndex();
    if (index.isValid()) {
        m_treeView->edit(index);
    }
}

void ProjectTree::onDeleteCell()
{
    QModelIndex index = m_treeView->currentIndex();
    if (index.isValid()) {
        QStandardItem *item = m_model->itemFromIndex(index);
        if (item && item->data(Qt::UserRole).toString() == "cell") {
            int ret = QMessageBox::question(this, tr("确认删除"),
                tr("确定要删除单元 \"%1\" 吗？").arg(item->text()),
                QMessageBox::Yes | QMessageBox::No);
            
            if (ret == QMessageBox::Yes) {
                // TODO: 删除单元
                m_model->removeRow(index.row(), index.parent());
            }
        }
    }
}

void ProjectTree::onRefresh()
{
    populateTree();
}
