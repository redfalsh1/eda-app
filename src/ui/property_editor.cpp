#include "ui/property_editor.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>

PropertyEditor::PropertyEditor(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
    
    setMinimumWidth(250);
}

PropertyEditor::~PropertyEditor()
{
}

void PropertyEditor::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // === 标题 ===
    m_titleLabel = new QLabel(tr("属性编辑器"), this);
    m_titleLabel->setStyleSheet(
        "QLabel { "
        "  background-color: #252526; "
        "  padding: 8px; "
        "  font-weight: bold; "
        "  font-size: 13px; "
        "  border-bottom: 1px solid #3e3e42; "
        "}"
    );
    mainLayout->addWidget(m_titleLabel);
    
    // === 属性树 ===
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setColumnCount(2);
    m_treeWidget->setHeaderLabels(QStringList() << tr("属性") << tr("值"));
    m_treeWidget->setRootIsDecorated(true);
    m_treeWidget->setAllColumnsShowFocus(true);
    m_treeWidget->setAlternatingRowColors(false);
    
    // 列宽
    m_treeWidget->setColumnWidth(0, 120);
    m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    
    mainLayout->addWidget(m_treeWidget);
    
    // === 空状态提示 ===
    m_emptyLabel = new QLabel(tr("未选中对象"), this);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet(
        "QLabel { "
        "  color: #858585; "
        "  font-size: 13px; "
        "  padding: 20px; "
        "}"
    );
    m_emptyLabel->hide();
    
    mainLayout->addWidget(m_emptyLabel);
}

void PropertyEditor::setupConnections()
{
    connect(m_treeWidget, &QTreeWidget::itemChanged,
            this, &PropertyEditor::onItemChanged);
}

void PropertyEditor::loadProperties(const QVariantMap &properties)
{
    m_currentProperties = properties;
    m_treeWidget->clear();
    
    if (properties.isEmpty()) {
        m_treeWidget->hide();
        m_emptyLabel->show();
        return;
    }
    
    m_treeWidget->show();
    m_emptyLabel->hide();
    
    // 添加几何属性
    if (properties.contains("geometry")) {
        QTreeWidgetItem *geoItem = new QTreeWidgetItem(m_treeWidget);
        geoItem->setText(0, tr("几何"));
        geoItem->setExpanded(true);
        
        QVariantMap geo = properties["geometry"].toMap();
        if (geo.contains("x")) {
            QTreeWidgetItem *xItem = new QTreeWidgetItem(geoItem);
            xItem->setText(0, "X");
            xItem->setText(1, QString::number(geo["x"].toDouble(), 'f', 2));
            xItem->setFlags(xItem->flags() | Qt::ItemIsEditable);
        }
        if (geo.contains("y")) {
            QTreeWidgetItem *yItem = new QTreeWidgetItem(geoItem);
            yItem->setText(0, "Y");
            yItem->setText(1, QString::number(geo["y"].toDouble(), 'f', 2));
            yItem->setFlags(yItem->flags() | Qt::ItemIsEditable);
        }
        if (geo.contains("width")) {
            QTreeWidgetItem *wItem = new QTreeWidgetItem(geoItem);
            wItem->setText(0, "宽度");
            wItem->setText(1, QString::number(geo["width"].toDouble(), 'f', 2));
            wItem->setFlags(wItem->flags() | Qt::ItemIsEditable);
        }
        if (geo.contains("height")) {
            QTreeWidgetItem *hItem = new QTreeWidgetItem(geoItem);
            hItem->setText(0, "高度");
            hItem->setText(1, QString::number(geo["height"].toDouble(), 'f', 2));
            hItem->setFlags(hItem->flags() | Qt::ItemIsEditable);
        }
    }
    
    // 添加图层属性
    if (properties.contains("layer")) {
        QTreeWidgetItem *layerItem = new QTreeWidgetItem(m_treeWidget);
        layerItem->setText(0, tr("图层"));
        
        QVariantMap layer = properties["layer"].toMap();
        if (layer.contains("id")) {
            QTreeWidgetItem *idItem = new QTreeWidgetItem(layerItem);
            idItem->setText(0, "ID");
            idItem->setText(1, QString::number(layer["id"].toInt()));
        }
        if (layer.contains("name")) {
            QTreeWidgetItem *nameItem = new QTreeWidgetItem(layerItem);
            nameItem->setText(0, "名称");
            nameItem->setText(1, layer["name"].toString());
        }
    }
    
    // 添加其他属性
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        if (it.key() != "geometry" && it.key() != "layer") {
            QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
            item->setText(0, it.key());
            item->setText(1, it.value().toString());
        }
    }
}

void PropertyEditor::clearProperties()
{
    m_currentProperties.clear();
    m_treeWidget->clear();
    m_treeWidget->hide();
    m_emptyLabel->show();
}

void PropertyEditor::refresh()
{
    loadProperties(m_currentProperties);
}

void PropertyEditor::onItemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    
    if (item && item->parent()) {
        QString propName = item->text(0);
        QString propValue = item->text(1);
        
        emit propertyChanged(propName, QVariant(propValue));
    }
}

void PropertyEditor::onSelectionChanged()
{
    // TODO: 响应画布选择变化
}
