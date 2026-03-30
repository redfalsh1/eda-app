#ifndef LAYOUT_LAYOUT_CELL_H
#define LAYOUT_LAYOUT_CELL_H

#include "layout/layout_types.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace layout {

// 前向声明
class LayoutDatabase;

/**
 * @brief 版图单元 (Cell)
 * 
 * 版图设计的基本组成单元，支持层次化嵌套
 * 一个 Cell 可以包含：
 * - 图形 (Polygon, Rect, Path, Text)
 * - 实例引用 (对其他 Cell 的引用)
 * - 图层信息
 */
class LayoutCell {
public:
    using ShapeList = std::vector<std::unique_ptr<Shape>>;
    using InstanceList = std::vector<std::unique_ptr<InstanceShape>>;
    using LayerMap = std::unordered_map<int32_t, std::shared_ptr<Layer>>;
    
    // ========================================================================
    // 构造与析构
    // ========================================================================
    
    LayoutCell() = default;
    explicit LayoutCell(std::string name);
    ~LayoutCell() = default;
    
    // 禁用拷贝
    LayoutCell(const LayoutCell&) = delete;
    LayoutCell& operator=(const LayoutCell&) = delete;
    
    // 允许移动
    LayoutCell(LayoutCell&&) noexcept = default;
    LayoutCell& operator=(LayoutCell&&) noexcept = default;
    
    // ========================================================================
    // 基本信息
    // ========================================================================
    
    const std::string& name() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    const std::string& viewName() const { return viewName_; }
    void setViewName(const std::string& name) { viewName_ = name; }
    
    // ========================================================================
    // 图形管理
    // ========================================================================
    
    /**
     * @brief 添加图形
     */
    void addShape(std::unique_ptr<Shape> shape);
    
    /**
     * @brief 添加多边形
     */
    void addPolygon(Polygon polygon, int32_t layerId);
    
    /**
     * @brief 添加矩形
     */
    void addRect(Rect rect, int32_t layerId);
    
    /**
     * @brief 添加路径
     */
    void addPath(Path path, int32_t layerId);
    
    /**
     * @brief 添加文本
     */
    void addText(Text text, int32_t layerId);
    
    /**
     * @brief 添加实例引用
     */
    void addInstance(CellInstance instance);
    
    /**
     * @brief 获取所有图形
     */
    const ShapeList& shapes() const { return shapes_; }
    
    /**
     * @brief 获取指定图层的图形
     */
    ShapeList shapesByLayer(int32_t layerId) const;
    
    /**
     * @brief 获取图形数量
     */
    size_t shapeCount() const { return shapes_.size(); }
    
    // ========================================================================
    // 实例管理
    // ========================================================================
    
    /**
     * @brief 获取所有实例
     */
    const InstanceList& instances() const { return instances_; }
    
    /**
     * @brief 获取实例数量
     */
    size_t instanceCount() const { return instances_.size(); }
    
    /**
     * @brief 获取引用的单元名列表
     */
    std::vector<std::string> referencedCells() const;
    
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
     * @brief 检查是否包含指定图层
     */
    bool hasLayer(int32_t layerId) const;
    
    // ========================================================================
    // 几何查询
    // ========================================================================
    
    /**
     * @brief 计算边界框
     */
    Rect boundingBox() const;
    
    /**
     * @brief 检查是否为空
     */
    bool isEmpty() const { return shapes_.empty() && instances_.empty(); }
    
    /**
     * @brief 查询指定区域内的图形
     */
    ShapeList queryShapes(const Rect& region) const;
    
    // ========================================================================
    // 变换
    // ========================================================================
    
    /**
     * @brief 平移所有图形
     */
    void translate(int64_t dx, int64_t dy);
    
    /**
     * @brief 旋转所有图形
     */
    void rotate(double angle, const Point& center = Point(0, 0));
    
    /**
     * @brief 镜像所有图形
     */
    void mirror(bool horizontal);
    
    // ========================================================================
    // 层次化遍历
    // ========================================================================
    
    /**
     * @brief 获取层次化图形总数 (递归)
     */
    size_t hierarchicalShapeCount() const;
    
    /**
     * @brief 获取层次化实例总数 (递归)
     */
    size_t hierarchicalInstanceCount() const;
    
    // ========================================================================
    // 克隆
    // ========================================================================
    
    /**
     * @brief 深拷贝
     */
    std::unique_ptr<LayoutCell> clone() const;

private:
    std::string name_;
    std::string viewName_;  // 视图名称 (layout, schematic, etc.)
    ShapeList shapes_;
    InstanceList instances_;
    LayerMap layers_;
};

} // namespace layout

#endif // LAYOUT_LAYOUT_CELL_H
