#ifndef LAYOUT_LAYOUT_DATABASE_H
#define LAYOUT_LAYOUT_DATABASE_H

#include "layout/layout_cell.h"
#include "layout/layout_types.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace layout {

/**
 * @brief 版图数据库
 * 
 * 顶层容器，管理所有单元 (Cell) 和全局信息
 * 支持：
 * - 层次化单元管理
 * - 图层定义
 * - 设计规则
 * - 元数据
 */
class LayoutDatabase {
public:
    using CellMap = std::unordered_map<std::string, std::unique_ptr<LayoutCell>>;
    using LayerMap = std::unordered_map<int32_t, std::shared_ptr<Layer>>;
    using CellVisitor = std::function<void(const std::string& name, const LayoutCell& cell)>;
    
    // ========================================================================
    // 构造与析构
    // ========================================================================
    
    LayoutDatabase() = default;
    ~LayoutDatabase() = default;
    
    // 禁用拷贝
    LayoutDatabase(const LayoutDatabase&) = delete;
    LayoutDatabase& operator=(const LayoutDatabase&) = delete;
    
    // 允许移动
    LayoutDatabase(LayoutDatabase&&) noexcept = default;
    LayoutDatabase& operator=(LayoutDatabase&&) noexcept = default;
    
    // ========================================================================
    // 基本信息
    // ========================================================================
    
    const std::string& libName() const { return libName_; }
    void setLibName(const std::string& name) { libName_ = name; }
    
    const std::string& topCellName() const { return topCellName_; }
    void setTopCellName(const std::string& name) { topCellName_ = name; }
    
    double databaseUnit() const { return databaseUnit_; }
    void setDatabaseUnit(double unit) { databaseUnit_ = unit; }
    
    // ========================================================================
    // 单元管理
    // ========================================================================
    
    /**
     * @brief 创建新单元
     */
    LayoutCell& createCell(const std::string& name);
    
    /**
     * @brief 获取单元
     */
    LayoutCell* getCell(const std::string& name);
    const LayoutCell* getCell(const std::string& name) const;
    
    /**
     * @brief 检查单元是否存在
     */
    bool hasCell(const std::string& name) const;
    
    /**
     * @brief 删除单元
     */
    bool deleteCell(const std::string& name);
    
    /**
     * @brief 获取所有单元名
     */
    std::vector<std::string> cellNames() const;
    
    /**
     * @brief 获取单元数量
     */
    size_t cellCount() const { return cells_.size(); }
    
    /**
     * @brief 获取顶层单元
     */
    LayoutCell* topCell();
    const LayoutCell* topCell() const;
    
    /**
     * @brief 遍历所有单元
     */
    void forEachCell(CellVisitor visitor) const;
    
    // ========================================================================
    // 层次化查询
    // ========================================================================
    
    /**
     * @brief 获取单元的所有引用者
     */
    std::vector<std::string> getCellReferences(const std::string& cellName) const;
    
    /**
     * @brief 检查是否有循环引用
     */
    bool hasCircularReference(const std::string& cellName) const;
    
    /**
     * @brief 获取层次化展开后的图形总数
     */
    size_t hierarchicalShapeCount(const std::string& cellName) const;
    
    /**
     * @brief 获取单元依赖列表 (拓扑排序)
     */
    std::vector<std::string> getCellDependencies(const std::string& cellName) const;
    
    // ========================================================================
    // 图层管理
    // ========================================================================
    
    /**
     * @brief 获取或创建图层
     */
    std::shared_ptr<Layer> getOrCreateLayer(int32_t layerId);
    
    /**
     * @brief 获取图层
     */
    std::shared_ptr<Layer> getLayer(int32_t layerId) const;
    
    /**
     * @brief 获取所有图层
     */
    const LayerMap& layers() const { return layers_; }
    
    /**
     * @brief 获取所有图层 ID
     */
    std::vector<int32_t> layerIds() const;
    
    /**
     * @brief 注册图层
     */
    void registerLayer(std::shared_ptr<Layer> layer);
    
    // ========================================================================
    // 几何查询
    // ========================================================================
    
    /**
     * @brief 查询指定区域内的图形 (扁平化)
     */
    std::vector<std::pair<std::string, std::unique_ptr<Shape>>> 
    queryShapes(const Rect& region) const;
    
    /**
     * @brief 查询指定图层的图形
     */
    std::vector<std::pair<std::string, std::unique_ptr<Shape>>> 
    queryShapesByLayer(int32_t layerId) const;
    
    // ========================================================================
    // 导入导出
    // ========================================================================
    
    /**
     * @brief 保存到文件
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief 从文件加载
     */
    static std::unique_ptr<LayoutDatabase> loadFromFile(const std::string& filepath);
    
    // ========================================================================
    // 统计信息
    // ========================================================================
    
    struct Statistics {
        size_t cellCount{0};
        size_t totalShapeCount{0};
        size_t totalInstanceCount{0};
        size_t layerCount{0};
        size_t uniqueLayerCount{0};
    };
    
    Statistics getStatistics() const;

private:
    std::string libName_;
    std::string topCellName_;
    double databaseUnit_{1e-9};  // 默认 1 纳米
    
    CellMap cells_;
    LayerMap layers_;
};

} // namespace layout

#endif // LAYOUT_LAYOUT_DATABASE_H
