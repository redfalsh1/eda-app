#include "layout/layout_cell.h"
#include <algorithm>
#include <set>

namespace layout {

// ============================================================================
// LayoutCell 实现
// ============================================================================

LayoutCell::LayoutCell(std::string name)
    : name_(std::move(name)) {
}

// ============================================================================
// 图形管理
// ============================================================================

void LayoutCell::addShape(std::unique_ptr<Shape> shape) {
    if (shape) {
        shapes_.push_back(std::move(shape));
    }
}

void LayoutCell::addPolygon(Polygon polygon, int32_t layerId) {
    if (polygon.isValid()) {
        auto shape = std::make_unique<PolygonShape>(std::move(polygon), layerId);
        shapes_.push_back(std::move(shape));
        
        // 确保图层存在
        getOrCreateLayer(layerId);
    }
}

void LayoutCell::addRect(Rect rect, int32_t layerId) {
    if (rect.isValid()) {
        auto shape = std::make_unique<RectShape>(rect, layerId);
        shapes_.push_back(std::move(shape));
        
        // 确保图层存在
        getOrCreateLayer(layerId);
    }
}

void LayoutCell::addPath(Path path, int32_t layerId) {
    if (path.pointCount() >= 2) {
        auto shape = std::make_unique<PathShape>(std::move(path), layerId);
        shapes_.push_back(std::move(shape));
        
        // 确保图层存在
        getOrCreateLayer(layerId);
    }
}

void LayoutCell::addText(Text text, int32_t layerId) {
    auto shape = std::make_unique<TextShape>(std::move(text), layerId);
    shapes_.push_back(std::move(shape));
    
    // 确保图层存在
    getOrCreateLayer(layerId);
}

void LayoutCell::addInstance(CellInstance instance) {
    auto shape = std::make_unique<InstanceShape>(std::move(instance));
    instances_.push_back(std::move(shape));
}

ShapeList LayoutCell::shapesByLayer(int32_t layerId) const {
    ShapeList result;
    for (const auto& shape : shapes_) {
        if (shape->layerId() == layerId) {
            result.push_back(shape->clone());
        }
    }
    return result;
}

// ============================================================================
// 实例管理
// ============================================================================

std::vector<std::string> LayoutCell::referencedCells() const {
    std::set<std::string> names;
    for (const auto& inst : instances_) {
        names.insert(inst->instance.cellName);
    }
    return std::vector<std::string>(names.begin(), names.end());
}

// ============================================================================
// 图层管理
// ============================================================================

std::shared_ptr<Layer> LayoutCell::getOrCreateLayer(int32_t layerId) {
    auto it = layers_.find(layerId);
    if (it != layers_.end()) {
        return it->second;
    }
    
    auto layer = std::make_shared<Layer>(layerId, "Layer_" + std::to_string(layerId));
    layers_[layerId] = layer;
    return layer;
}

std::shared_ptr<Layer> LayoutCell::getLayer(int32_t layerId) const {
    auto it = layers_.find(layerId);
    return (it != layers_.end()) ? it->second : nullptr;
}

std::vector<int32_t> LayoutCell::layerIds() const {
    std::vector<int32_t> ids;
    ids.reserve(layers_.size());
    for (const auto& [id, layer] : layers_) {
        ids.push_back(id);
    }
    return ids;
}

bool LayoutCell::hasLayer(int32_t layerId) const {
    return layers_.count(layerId) > 0;
}

// ============================================================================
// 几何查询
// ============================================================================

Rect LayoutCell::boundingBox() const {
    if (shapes_.empty() && instances_.empty()) {
        return Rect(0, 0, 0, 0);
    }
    
    int64_t minX = INT64_MAX;
    int64_t minY = INT64_MAX;
    int64_t maxX = INT64_MIN;
    int64_t maxY = INT64_MIN;
    
    bool hasContent = false;
    
    // 计算图形的边界框
    for (const auto& shape : shapes_) {
        Rect box = shape->boundingBox();
        if (box.isValid()) {
            minX = std::min(minX, box.left);
            minY = std::min(minY, box.bottom);
            maxX = std::max(maxX, box.right);
            maxY = std::max(maxY, box.top);
            hasContent = true;
        }
    }
    
    // 实例的边界框需要递归计算，这里简化处理
    // 实际应用中需要从数据库中获取引用的 Cell
    
    return hasContent ? Rect(minX, minY, maxX, maxY) : Rect(0, 0, 0, 0);
}

ShapeList LayoutCell::queryShapes(const Rect& region) const {
    ShapeList result;
    for (const auto& shape : shapes_) {
        if (shape->boundingBox().intersects(region)) {
            result.push_back(shape->clone());
        }
    }
    return result;
}

// ============================================================================
// 变换
// ============================================================================

void LayoutCell::translate(int64_t dx, int64_t dy) {
    for (auto& shape : shapes_) {
        shape->translate(dx, dy);
    }
    for (auto& inst : instances_) {
        inst->translate(dx, dy);
    }
}

void LayoutCell::rotate(double angle, const Point& center) {
    for (auto& shape : shapes_) {
        shape->rotate(angle, center);
    }
    for (auto& inst : instances_) {
        inst->rotate(angle, center);
    }
}

void LayoutCell::mirror(bool horizontal) {
    Point center = boundingBox().center();
    
    for (auto& shape : shapes_) {
        shape->mirror(horizontal);
    }
    for (auto& inst : instances_) {
        inst->mirror(horizontal);
    }
}

// ============================================================================
// 层次化遍历
// ============================================================================

size_t LayoutCell::hierarchicalShapeCount() const {
    size_t count = shapes_.size();
    
    // 注意：这里无法递归计算实例中的图形数量
    // 因为 LayoutCell 不知道父数据库，实际应用中需要通过数据库来递归
    
    return count;
}

size_t LayoutCell::hierarchicalInstanceCount() const {
    size_t count = instances_.size();
    
    // 同样，无法递归计算实例的实例
    // 需要通过数据库来递归
    
    return count;
}

// ============================================================================
// 克隆
// ============================================================================

std::unique_ptr<LayoutCell> LayoutCell::clone() const {
    auto cloned = std::make_unique<LayoutCell>(name_);
    cloned->viewName_ = viewName_;
    
    // 深拷贝图形
    for (const auto& shape : shapes_) {
        cloned->shapes_.push_back(shape->clone());
    }
    
    // 深拷贝实例
    for (const auto& inst : instances_) {
        cloned->instances_.push_back(inst->clone());
    }
    
    // 拷贝图层
    for (const auto& [id, layer] : layers_) {
        auto clonedLayer = std::make_shared<Layer>(*layer);
        cloned->layers_[id] = clonedLayer;
    }
    
    return cloned;
}

} // namespace layout
