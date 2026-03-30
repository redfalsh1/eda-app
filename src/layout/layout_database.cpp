#include "layout/layout_database.h"
#include <algorithm>
#include <set>
#include <stack>
#include <fstream>

namespace layout {

// ============================================================================
// LayoutDatabase 实现
// ============================================================================

// -----------------------------------------------------------------------------
// 单元管理
// -----------------------------------------------------------------------------

LayoutCell& LayoutDatabase::createCell(const std::string& name) {
    auto it = cells_.find(name);
    if (it != cells_.end()) {
        return *it->second;
    }
    
    auto cell = std::make_unique<LayoutCell>(name);
    auto& ref = *cell;
    cells_[name] = std::move(cell);
    return ref;
}

LayoutCell* LayoutDatabase::getCell(const std::string& name) {
    auto it = cells_.find(name);
    return (it != cells_.end()) ? it->second.get() : nullptr;
}

const LayoutCell* LayoutDatabase::getCell(const std::string& name) const {
    auto it = cells_.find(name);
    return (it != cells_.end()) ? it->second.get() : nullptr;
}

bool LayoutDatabase::hasCell(const std::string& name) const {
    return cells_.count(name) > 0;
}

bool LayoutDatabase::deleteCell(const std::string& name) {
    // 检查是否有其他单元引用此单元
    if (!getCellReferences(name).empty()) {
        // 有引用，不能删除
        return false;
    }
    
    return cells_.erase(name) > 0;
}

std::vector<std::string> LayoutDatabase::cellNames() const {
    std::vector<std::string> names;
    names.reserve(cells_.size());
    for (const auto& [name, cell] : cells_) {
        names.push_back(name);
    }
    return names;
}

LayoutCell* LayoutDatabase::topCell() {
    if (topCellName_.empty()) {
        return nullptr;
    }
    return getCell(topCellName_);
}

const LayoutCell* LayoutDatabase::topCell() const {
    if (topCellName_.empty()) {
        return nullptr;
    }
    return getCell(topCellName_);
}

void LayoutDatabase::forEachCell(CellVisitor visitor) const {
    for (const auto& [name, cell] : cells_) {
        visitor(name, *cell);
    }
}

// -----------------------------------------------------------------------------
// 层次化查询
// -----------------------------------------------------------------------------

std::vector<std::string> LayoutDatabase::getCellReferences(const std::string& cellName) const {
    std::vector<std::string> references;
    
    for (const auto& [name, cell] : cells_) {
        if (name == cellName) continue;
        
        auto refs = cell->referencedCells();
        if (std::find(refs.begin(), refs.end(), cellName) != refs.end()) {
            references.push_back(name);
        }
    }
    
    return references;
}

bool LayoutDatabase::hasCircularReference(const std::string& cellName) const {
    auto cell = getCell(cellName);
    if (!cell) return false;
    
    std::set<std::string> visited;
    std::stack<std::string> toVisit;
    
    // 获取直接引用的单元
    for (const auto& ref : cell->referencedCells()) {
        toVisit.push(ref);
    }
    
    while (!toVisit.empty()) {
        std::string current = toVisit.top();
        toVisit.pop();
        
        if (current == cellName) {
            return true;  // 找到循环引用
        }
        
        if (visited.count(current)) {
            continue;
        }
        visited.insert(current);
        
        auto refCell = getCell(current);
        if (refCell) {
            for (const auto& ref : refCell->referencedCells()) {
                toVisit.push(ref);
            }
        }
    }
    
    return false;
}

size_t LayoutDatabase::hierarchicalShapeCount(const std::string& cellName) const {
    auto cell = getCell(cellName);
    if (!cell) return 0;
    
    size_t count = 0;
    std::stack<std::string> toVisit;
    std::set<std::string> visited;
    
    toVisit.push(cellName);
    
    while (!toVisit.empty()) {
        std::string current = toVisit.top();
        toVisit.pop();
        
        if (visited.count(current)) {
            continue;
        }
        visited.insert(current);
        
        auto c = getCell(current);
        if (!c) continue;
        
        // 计算当前单元的图形数
        count += c->shapeCount();
        
        // 计算实例数并添加到待访问列表
        for (const auto& inst : c->instances()) {
            count++;  // 实例本身也算
            toVisit.push(inst->instance.cellName);
        }
    }
    
    return count;
}

std::vector<std::string> LayoutDatabase::getCellDependencies(const std::string& cellName) const {
    std::vector<std::string> dependencies;
    std::set<std::string> visited;
    std::stack<std::string> toVisit;
    
    toVisit.push(cellName);
    
    while (!toVisit.empty()) {
        std::string current = toVisit.top();
        toVisit.pop();
        
        if (visited.count(current)) {
            continue;
        }
        visited.insert(current);
        
        auto cell = getCell(current);
        if (!cell) continue;
        
        for (const auto& ref : cell->referencedCells()) {
            if (!visited.count(ref)) {
                dependencies.push_back(ref);
                toVisit.push(ref);
            }
        }
    }
    
    // 反转顺序，使依赖项在前
    std::reverse(dependencies.begin(), dependencies.end());
    return dependencies;
}

// -----------------------------------------------------------------------------
// 图层管理
// -----------------------------------------------------------------------------

std::shared_ptr<Layer> LayoutDatabase::getOrCreateLayer(int32_t layerId) {
    auto it = layers_.find(layerId);
    if (it != layers_.end()) {
        return it->second;
    }
    
    auto layer = std::make_shared<Layer>(layerId, "Layer_" + std::to_string(layerId));
    layers_[layerId] = layer;
    return layer;
}

std::shared_ptr<Layer> LayoutDatabase::getLayer(int32_t layerId) const {
    auto it = layers_.find(layerId);
    return (it != layers_.end()) ? it->second : nullptr;
}

std::vector<int32_t> LayoutDatabase::layerIds() const {
    std::vector<int32_t> ids;
    ids.reserve(layers_.size());
    for (const auto& [id, layer] : layers_) {
        ids.push_back(id);
    }
    return ids;
}

void LayoutDatabase::registerLayer(std::shared_ptr<Layer> layer) {
    if (layer) {
        layers_[layer->id] = std::move(layer);
    }
}

// -----------------------------------------------------------------------------
// 几何查询
// -----------------------------------------------------------------------------

std::vector<std::pair<std::string, std::unique_ptr<Shape>>> 
LayoutDatabase::queryShapes(const Rect& region) const {
    std::vector<std::pair<std::string, std::unique_ptr<Shape>>> result;
    
    for (const auto& [cellName, cell] : cells_) {
        auto shapes = cell->queryShapes(region);
        for (auto& shape : shapes) {
            result.emplace_back(cellName, std::move(shape));
        }
    }
    
    return result;
}

std::vector<std::pair<std::string, std::unique_ptr<Shape>>> 
LayoutDatabase::queryShapesByLayer(int32_t layerId) const {
    std::vector<std::pair<std::string, std::unique_ptr<Shape>>> result;
    
    for (const auto& [cellName, cell] : cells_) {
        auto shapes = cell->shapesByLayer(layerId);
        for (auto& shape : shapes) {
            result.emplace_back(cellName, std::move(shape));
        }
    }
    
    return result;
}

// -----------------------------------------------------------------------------
// 导入导出
// -----------------------------------------------------------------------------

bool LayoutDatabase::saveToFile(const std::string& filepath) const {
    // TODO: 实现序列化保存
    // 这里只是一个占位实现
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "LayoutDatabase: " << libName_ << "\n";
    file << "Top Cell: " << topCellName_ << "\n";
    file << "Cell Count: " << cells_.size() << "\n";
    
    return true;
}

std::unique_ptr<LayoutDatabase> LayoutDatabase::loadFromFile(const std::string& filepath) {
    // TODO: 实现反序列化加载
    // 这里只是一个占位实现
    auto db = std::make_unique<LayoutDatabase>();
    return db;
}

// -----------------------------------------------------------------------------
// 统计信息
// -----------------------------------------------------------------------------

LayoutDatabase::Statistics LayoutDatabase::getStatistics() const {
    Statistics stats;
    stats.cellCount = cells_.size();
    stats.layerCount = layers_.size();
    
    std::set<int32_t> uniqueLayers;
    
    for (const auto& [name, cell] : cells_) {
        stats.totalShapeCount += cell->shapeCount();
        stats.totalInstanceCount += cell->instanceCount();
        
        for (int32_t layerId : cell->layerIds()) {
            uniqueLayers.insert(layerId);
        }
    }
    
    stats.uniqueLayerCount = uniqueLayers.size();
    
    return stats;
}

} // namespace layout
