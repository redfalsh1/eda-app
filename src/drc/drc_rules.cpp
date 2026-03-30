#include "drc/drc_types.h"
#include "layout/layout_database.h"
#include <algorithm>
#include <cmath>

namespace drc {

// ============================================================================
// MinSpacingRule 实现
// ============================================================================

std::vector<DRCError> MinSpacingRule::check(const void* database) const {
    std::vector<DRCError> errors;
    
    if (!enabled_) return errors;
    
    const auto* db = static_cast<const layout::LayoutDatabase*>(database);
    if (!db) return errors;
    
    // 获取两个图层的所有图形
    auto shapes1 = db->queryShapesByLayer(layer1_);
    auto shapes2 = db->queryShapesByLayer(layer2_);
    
    // 检查每对图形之间的间距
    for (const auto& [cell1, shape1] : shapes1) {
        layout::Rect box1 = shape1->boundingBox();
        
        for (const auto& [cell2, shape2] : shapes2) {
            // 同一单元同一图层的图形跳过自检查
            if (cell1 == cell2 && layer1_ == layer2_) {
                if (shape1.get() == shape2.get()) continue;
            }
            
            layout::Rect box2 = shape2->boundingBox();
            
            // 计算边界框之间的最小距离
            int64_t dx = 0, dy = 0;
            
            if (box1.right < box2.left) {
                dx = box2.left - box1.right;
            } else if (box1.left > box2.right) {
                dx = box1.left - box2.right;
            }
            
            if (box1.top < box2.bottom) {
                dy = box2.bottom - box1.top;
            } else if (box1.bottom > box2.top) {
                dy = box1.bottom - box2.top;
            }
            
            int64_t distance = std::max(dx, dy);
            
            // 如果边界框重叠，需要更精确的检查
            if (dx == 0 && dy == 0 && box1.intersects(box2)) {
                DRCError error(type(), severity(), name(),
                    QString("Overlap detected between %1 and %2 in cell %3")
                        .arg(layer1_).arg(layer2_).arg(QString::fromStdString(cell2)));
                error.boundingBox = box1.intersect(box2);
                error.layerId1 = layer1_;
                error.layerId2 = layer2_;
                errors.push_back(error);
                continue;
            }
            
            // 检查间距
            if (distance < minSpace_ && distance >= 0) {
                DRCError error(type(), severity(), name(),
                    QString("Spacing violation: %1 nm (minimum: %2 nm) between layers %3 and %4")
                        .arg(distance).arg(minSpace_).arg(layer1_).arg(layer2_));
                error.boundingBox = box1.unite(box2);
                error.layerId1 = layer1_;
                error.layerId2 = layer2_;
                error.expectedValue = minSpace_;
                error.actualValue = distance;
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

// ============================================================================
// MinWidthRule 实现
// ============================================================================

std::vector<DRCError> MinWidthRule::check(const void* database) const {
    std::vector<DRCError> errors;
    
    if (!enabled_) return errors;
    
    const auto* db = static_cast<const layout::LayoutDatabase*>(database);
    if (!db) return errors;
    
    auto shapes = db->queryShapesByLayer(layer_);
    
    for (const auto& [cellName, shape] : shapes) {
        layout::Rect box = shape->boundingBox();
        
        // 检查宽度和高度
        int64_t width = box.width();
        int64_t height = box.height();
        
        int64_t minWidth = std::min(width, height);
        
        if (minWidth < minWidth_ && minWidth > 0) {
            DRCError error(type(), severity(), name(),
                QString("Width violation: %1 nm (minimum: %2 nm) on layer %3 in cell %4")
                    .arg(minWidth).arg(minWidth_).arg(layer_).arg(QString::fromStdString(cellName)));
            error.boundingBox = box;
            error.layerId1 = layer_;
            error.expectedValue = minWidth_;
            error.actualValue = minWidth;
            errors.push_back(error);
        }
    }
    
    return errors;
}

// ============================================================================
// MinEnclosureRule 实现
// ============================================================================

std::vector<DRCError> MinEnclosureRule::check(const void* database) const {
    std::vector<DRCError> errors;
    
    if (!enabled_) return errors;
    
    const auto* db = static_cast<const layout::LayoutDatabase*>(database);
    if (!db) return errors;
    
    auto innerShapes = db->queryShapesByLayer(innerLayer_);
    auto outerShapes = db->queryShapesByLayer(outerLayer_);
    
    for (const auto& [cellName, innerShape] : innerShapes) {
        layout::Rect innerBox = innerShape->boundingBox();
        
        bool isEnclosed = false;
        
        for (const auto& [outerCell, outerShape] : outerShapes) {
            if (cellName != outerCell) continue;
            
            layout::Rect outerBox = outerShape->boundingBox();
            
            // 检查包围
            int64_t leftEnc = innerBox.left - outerBox.left;
            int64_t rightEnc = outerBox.right - innerBox.right;
            int64_t bottomEnc = innerBox.bottom - outerBox.bottom;
            int64_t topEnc = outerBox.top - innerBox.top;
            
            int64_t minEnc = std::min({leftEnc, rightEnc, bottomEnc, topEnc});
            
            if (minEnc >= minEnclosure_) {
                isEnclosed = true;
                break;
            }
        }
        
        if (!isEnclosed) {
            DRCError error(type(), severity(), name(),
                QString("Enclosure violation: layer %1 not properly enclosed by layer %2 in cell %3")
                    .arg(innerLayer_).arg(outerLayer_).arg(QString::fromStdString(cellName)));
            error.boundingBox = innerBox;
            error.layerId1 = innerLayer_;
            error.layerId2 = outerLayer_;
            error.expectedValue = minEnclosure_;
            errors.push_back(error);
        }
    }
    
    return errors;
}

// ============================================================================
// MinAreaRule 实现
// ============================================================================

std::vector<DRCError> MinAreaRule::check(const void* database) const {
    std::vector<DRCError> errors;
    
    if (!enabled_) return errors;
    
    const auto* db = static_cast<const layout::LayoutDatabase*>(database);
    if (!db) return errors;
    
    auto shapes = db->queryShapesByLayer(layer_);
    
    for (const auto& [cellName, shape] : shapes) {
        layout::Rect box = shape->boundingBox();
        int64_t area = box.area();
        
        if (area < minArea_ && area > 0) {
            DRCError error(type(), severity(), name(),
                QString("Area violation: %1 sq.nm (minimum: %2 sq.nm) on layer %3 in cell %4")
                    .arg(area).arg(minArea_).arg(layer_).arg(QString::fromStdString(cellName)));
            error.boundingBox = box;
            error.layerId1 = layer_;
            error.expectedValue = minArea_;
            error.actualValue = area;
            errors.push_back(error);
        }
    }
    
    return errors;
}

// ============================================================================
// OverlapRule 实现
// ============================================================================

std::vector<DRCError> OverlapRule::check(const void* database) const {
    std::vector<DRCError> errors;
    
    if (!enabled_) return errors;
    
    const auto* db = static_cast<const layout::LayoutDatabase*>(database);
    if (!db) return errors;
    
    auto shapes1 = db->queryShapesByLayer(layer1_);
    auto shapes2 = db->queryShapesByLayer(layer2_);
    
    if (allowOverlap_) {
        // 检查必须重叠的情况
        for (const auto& [cell1, shape1] : shapes1) {
            bool hasOverlap = false;
            layout::Rect box1 = shape1->boundingBox();
            
            for (const auto& [cell2, shape2] : shapes2) {
                if (cell1 != cell2) continue;
                
                layout::Rect box2 = shape2->boundingBox();
                
                if (box1.intersects(box2)) {
                    hasOverlap = true;
                    break;
                }
            }
            
            if (!hasOverlap) {
                DRCError error(type(), severity(), name(),
                    QString("Missing overlap: layer %1 should overlap with layer %2 in cell %3")
                        .arg(layer1_).arg(layer2_).arg(QString::fromStdString(cell1)));
                error.boundingBox = box1;
                error.layerId1 = layer1_;
                error.layerId2 = layer2_;
                errors.push_back(error);
            }
        }
    } else {
        // 检查不允许重叠的情况
        for (const auto& [cell1, shape1] : shapes1) {
            layout::Rect box1 = shape1->boundingBox();
            
            for (const auto& [cell2, shape2] : shapes2) {
                if (cell1 != cell2) continue;
                if (layer1_ == layer2_ && shape1.get() == shape2.get()) continue;
                
                layout::Rect box2 = shape2->boundingBox();
                
                if (box1.intersects(box2)) {
                    DRCError error(type(), severity(), name(),
                        QString("Overlap detected between layer %1 and %2 in cell %3")
                            .arg(layer1_).arg(layer2_).arg(QString::fromStdString(cell1)));
                    error.boundingBox = box1.intersect(box2);
                    error.layerId1 = layer1_;
                    error.layerId2 = layer2_;
                    errors.push_back(error);
                }
            }
        }
    }
    
    return errors;
}

} // namespace drc
