#ifndef UI_PROPERTY_EDITOR_H
#define UI_PROPERTY_EDITOR_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>

/**
 * @brief 属性编辑器控件
 * 
 * 显示和编辑选中对象的属性：
 * - 几何属性（坐标/尺寸）
 * - 图层属性
 * - 电气属性
 */
class PropertyEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent = nullptr);
    ~PropertyEditor();

    // 加载属性
    void loadProperties(const QVariantMap &properties);
    void clearProperties();
    
    // 刷新
    void refresh();

signals:
    void propertyChanged(const QString &name, const QVariant &value);

private slots:
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onSelectionChanged();

private:
    void setupUI();
    void setupConnections();
    void addProperty(const QString &name, const QVariant &value, QTreeWidgetItem *parent = nullptr);

    QTreeWidget *m_treeWidget;
    QLabel *m_titleLabel;
    QLabel *m_emptyLabel;
    
    QVariantMap m_currentProperties;
};

#endif // UI_PROPERTY_EDITOR_H
