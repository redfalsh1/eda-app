#include "layout/layout_types.h"
#include <algorithm>
#include <cmath>

namespace layout {

// ============================================================================
// Polygon 实现
// ============================================================================

Rect Polygon::boundingBox() const {
    return calculateBoundingBox(points);
}

// ============================================================================
// CellInstance 实现
// ============================================================================

Point CellInstance::transform(const Point& p) const {
    Point result = p;
    
    // 应用缩放
    if (magnification != 1) {
        result.x = result.x * magnification / 1000;
        result.y = result.y * magnification / 1000;
    }
    
    // 应用镜像
    if (mirrorX) {
        result.x = -result.x;
    }
    if (mirrorY) {
        result.y = -result.y;
    }
    
    // 应用旋转
    if (rotation != 0.0) {
        double rad = rotation * M_PI / 180.0;
        double cosA = std::cos(rad);
        double sinA = std::sin(rad);
        
        int64_t newX = static_cast<int64_t>(result.x * cosA - result.y * sinA);
        int64_t newY = static_cast<int64_t>(result.x * sinA + result.y * cosA);
        result.x = newX;
        result.y = newY;
    }
    
    // 应用平移
    result.x += position.x;
    result.y += position.y;
    
    return result;
}

// ============================================================================
// Shape 派生类实现
// ============================================================================

Rect PolygonShape::boundingBox() const {
    return polygon.boundingBox();
}

std::unique_ptr<Shape> PolygonShape::clone() const {
    return std::make_unique<PolygonShape>(*this);
}

void PolygonShape::translate(int64_t dx, int64_t dy) {
    for (auto& p : polygon.points) {
        p.x += dx;
        p.y += dy;
    }
}

void PolygonShape::rotate(double angle, const Point& center) {
    for (auto& p : polygon.points) {
        p = rotatePoint(p, angle, center);
    }
}

void PolygonShape::mirror(bool horizontal) {
    Point center = polygon.boundingBox().center();
    for (auto& p : polygon.points) {
        p = mirrorPoint(p, horizontal, center);
    }
}

// -----------------------------------------------------------------------------

std::unique_ptr<Shape> RectShape::clone() const {
    return std::make_unique<RectShape>(*this);
}

void RectShape::rotate(double angle, const Point& center) {
    if (std::fmod(angle, 360.0) == 0.0) return;
    
    // 矩形旋转后可能变成多边形，这里简化处理
    // 实际应用中可能需要转换为 Polygon
    Point corners[4] = {
        Point(rect.left, rect.bottom),
        Point(rect.right, rect.bottom),
        Point(rect.right, rect.top),
        Point(rect.left, rect.top)
    };
    
    for (auto& p : corners) {
        p = rotatePoint(p, angle, center);
    }
    
    // 重新计算边界框
    Rect newBox = calculateBoundingBox(std::vector<Point>(corners, corners + 4));
    rect = newBox;
}

void RectShape::mirror(bool horizontal) {
    Point center = rect.center();
    
    if (horizontal) {
        int64_t oldLeft = rect.left;
        rect.left = 2 * center.x - rect.right;
        rect.right = 2 * center.x - oldLeft;
    } else {
        int64_t oldBottom = rect.bottom;
        rect.bottom = 2 * center.y - rect.top;
        rect.top = 2 * center.y - oldBottom;
    }
}

// -----------------------------------------------------------------------------

Rect PathShape::boundingBox() const {
    if (points.empty()) return Rect();
    
    Rect box = calculateBoundingBox(points);
    
    // 扩展路径宽度
    int64_t halfWidth = width / 2;
    return Rect(
        box.left - halfWidth,
        box.bottom - halfWidth,
        box.right + halfWidth,
        box.top + halfWidth
    );
}

std::unique_ptr<Shape> PathShape::clone() const {
    return std::make_unique<PathShape>(*this);
}

void PathShape::translate(int64_t dx, int64_t dy) {
    for (auto& p : path.points) {
        p.x += dx;
        p.y += dy;
    }
}

void PathShape::rotate(double angle, const Point& center) {
    for (auto& p : path.points) {
        p = rotatePoint(p, angle, center);
    }
}

void PathShape::mirror(bool horizontal) {
    Point center = boundingBox().center();
    for (auto& p : path.points) {
        p = mirrorPoint(p, horizontal, center);
    }
}

// -----------------------------------------------------------------------------

Rect TextShape::boundingBox() const {
    // 简化：假设文本宽度约为高度的 2/3 * 字符数
    int64_t width = static_cast<int64_t>(text.string.length() * text.height * 2 / 3);
    int64_t height = text.height;
    
    int64_t left = text.position.x;
    int64_t bottom = text.position.y;
    
    // 根据对齐方式调整
    if (text.justification == 1) {  // center
        left -= width / 2;
    } else if (text.justification == 2) {  // right
        left -= width;
    }
    
    return Rect(left, bottom, left + width, bottom + height);
}

std::unique_ptr<Shape> TextShape::clone() const {
    return std::make_unique<TextShape>(*this);
}

void TextShape::rotate(double angle, const Point& center) {
    text.position = rotatePoint(text.position, angle, center);
}

void TextShape::mirror(bool horizontal) {
    Point center = boundingBox().center();
    text.position = mirrorPoint(text.position, horizontal, center);
}

// -----------------------------------------------------------------------------

Rect InstanceShape::boundingBox() const {
    // 实例的边界框需要递归计算引用的单元
    // 这里返回一个空矩形，实际使用时需要从数据库中获取
    return Rect();
}

std::unique_ptr<Shape> InstanceShape::clone() const {
    return std::make_unique<InstanceShape>(*this);
}

void InstanceShape::rotate(double angle, const Point& center) {
    // 实例旋转需要特殊处理
    Point newPos = rotatePoint(instance.position, angle, center);
    instance.position = newPos;
    instance.rotation = std::fmod(instance.rotation + angle, 360.0);
}

void InstanceShape::mirror(bool horizontal) {
    Point center = boundingBox().center();
    instance.position = mirrorPoint(instance.position, horizontal, center);
    instance.mirrorX = !instance.mirrorX;
}

// ============================================================================
// 工具函数实现
// ============================================================================

Rect calculateBoundingBox(const std::vector<Point>& points) {
    if (points.empty()) {
        return Rect(0, 0, 0, 0);
    }
    
    int64_t minX = points[0].x;
    int64_t minY = points[0].y;
    int64_t maxX = points[0].x;
    int64_t maxY = points[0].y;
    
    for (const auto& p : points) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
    }
    
    return Rect(minX, minY, maxX, maxY);
}

Point rotatePoint(const Point& p, double angle, const Point& center) {
    if (angle == 0.0) return p;
    
    double rad = angle * M_PI / 180.0;
    double cosA = std::cos(rad);
    double sinA = std::sin(rad);
    
    int64_t dx = p.x - center.x;
    int64_t dy = p.y - center.y;
    
    int64_t newX = static_cast<int64_t>(dx * cosA - dy * sinA);
    int64_t newY = static_cast<int64_t>(dx * sinA + dy * cosA);
    
    return Point(center.x + newX, center.y + newY);
}

Point mirrorPoint(const Point& p, bool horizontal, const Point& center) {
    if (horizontal) {
        return Point(2 * center.x - p.x, p.y);
    } else {
        return Point(p.x, 2 * center.y - p.y);
    }
}

} // namespace layout
