#ifndef LAYOUT_LAYOUT_TYPES_H
#define LAYOUT_LAYOUT_TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace layout {

// ============================================================================
// 基础类型定义
// ============================================================================

/**
 * @brief 坐标点 (纳米级精度)
 */
struct Point {
    int64_t x{0};
    int64_t y{0};
    
    Point() = default;
    Point(int64_t x_, int64_t y_) : x(x_), y(y_) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    Point& operator+=(const Point& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Point operator*(int64_t scalar) const {
        return Point(x * scalar, y * scalar);
    }
};

/**
 * @brief 矩形 (轴对齐)
 */
struct Rect {
    int64_t left{0};
    int64_t bottom{0};
    int64_t right{0};
    int64_t top{0};
    
    Rect() = default;
    Rect(int64_t l, int64_t b, int64_t r, int64_t t)
        : left(l), bottom(b), right(r), top(t) {}
    
    Rect(const Point& min, const Point& max)
        : left(min.x), bottom(min.y), right(max.x), top(max.y) {}
    
    int64_t width() const { return right - left; }
    int64_t height() const { return top - bottom; }
    int64_t area() const { return width() * height(); }
    
    Point center() const {
        return Point((left + right) / 2, (bottom + top) / 2);
    }
    
    Point minPoint() const { return Point(left, bottom); }
    Point maxPoint() const { return Point(right, top); }
    
    bool contains(const Point& p) const {
        return p.x >= left && p.x <= right &&
               p.y >= bottom && p.y <= top;
    }
    
    bool intersects(const Rect& other) const {
        return left <= other.right && right >= other.left &&
               bottom <= other.top && top >= other.bottom;
    }
    
    Rect intersect(const Rect& other) const {
        return Rect(
            std::max(left, other.left),
            std::max(bottom, other.bottom),
            std::min(right, other.right),
            std::min(top, other.top)
        );
    }
    
    Rect unite(const Rect& other) const {
        return Rect(
            std::min(left, other.left),
            std::min(bottom, other.bottom),
            std::max(right, other.right),
            std::max(top, other.top)
        );
    }
    
    void translate(int64_t dx, int64_t dy) {
        left += dx; right += dx;
        bottom += dy; top += dy;
    }
    
    bool isValid() const {
        return left <= right && bottom <= top;
    }
};

/**
 * @brief 多边形
 */
struct Polygon {
    std::vector<Point> points;
    
    Polygon() = default;
    explicit Polygon(std::vector<Point> pts) : points(std::move(pts)) {}
    
    void addPoint(int64_t x, int64_t y) {
        points.emplace_back(x, y);
    }
    
    void addPoint(const Point& p) {
        points.push_back(p);
    }
    
    size_t pointCount() const { return points.size(); }
    
    bool isValid() const { return points.size() >= 3; }
    
    Rect boundingBox() const;
};

/**
 * @brief 路径
 */
struct Path {
    std::vector<Point> points;
    int64_t width{0};
    
    Path() = default;
    Path(std::vector<Point> pts, int64_t w)
        : points(std::move(pts)), width(w) {}
    
    void addPoint(int64_t x, int64_t y) {
        points.emplace_back(x, y);
    }
    
    size_t pointCount() const { return points.size(); }
};

/**
 * @brief 文本
 */
struct Text {
    std::string string;
    Point position;
    int64_t height{0};
    int32_t layerId{0};
    int32_t justification{0};  // 0=left, 1=center, 2=right
    
    Text() = default;
    Text(std::string str, Point pos, int64_t h)
        : string(std::move(str)), position(pos), height(h) {}
};

// ============================================================================
// 图层定义
// ============================================================================

/**
 * @brief 图层信息
 */
struct Layer {
    int32_t id{0};
    std::string name;
    std::string dataType;  // "drawing", "label", "pin", etc.
    
    // 显示属性
    int32_t fillPattern{0};
    int32_t lineStyle{0};
    uint8_t red{0};
    uint8_t green{0};
    uint8_t blue{0};
    uint8_t alpha{255};
    
    Layer() = default;
    Layer(int32_t id_, std::string name_)
        : id(id_), name(std::move(name_)) {}
};

// ============================================================================
// 单元实例引用
// ============================================================================

/**
 * @brief 单元实例 (Cell Instance)
 * 
 * 对一个单元的引用，包含变换信息
 */
struct CellInstance {
    std::string cellName;       // 引用的单元名
    Point position;             // 位置
    double rotation{0.0};       // 旋转角度 (度)
    bool mirrorX{false};        // X 轴镜像
    bool mirrorY{false};        // Y 轴镜像
    int64_t magnification{1};   // 缩放倍数 (千分之一单位)
    
    CellInstance() = default;
    CellInstance(std::string name, Point pos)
        : cellName(std::move(name)), position(pos) {}
    
    /**
     * @brief 应用变换到点
     */
    Point transform(const Point& p) const;
};

// ============================================================================
// 图形基类和派生类
// ============================================================================

/**
 * @brief 图形类型枚举
 */
enum class ShapeType {
    Unknown = 0,
    Polygon,
    Rect,
    Path,
    Text,
    Instance
};

/**
 * @brief 图形基类
 */
class Shape {
public:
    virtual ~Shape() = default;
    
    virtual ShapeType type() const = 0;
    virtual int32_t layerId() const = 0;
    virtual Rect boundingBox() const = 0;
    virtual std::unique_ptr<Shape> clone() const = 0;
    
    virtual void translate(int64_t dx, int64_t dy) = 0;
    virtual void rotate(double angle, const Point& center) = 0;
    virtual void mirror(bool horizontal) = 0;
};

/**
 * @brief 多边形图形
 */
class PolygonShape : public Shape {
public:
    Polygon polygon;
    int32_t layerId_{0};
    
    PolygonShape() = default;
    PolygonShape(Polygon poly, int32_t layer)
        : polygon(std::move(poly)), layerId_(layer) {}
    
    ShapeType type() const override { return ShapeType::Polygon; }
    int32_t layerId() const override { return layerId_; }
    Rect boundingBox() const override;
    std::unique_ptr<Shape> clone() const override;
    
    void translate(int64_t dx, int64_t dy) override;
    void rotate(double angle, const Point& center) override;
    void mirror(bool horizontal) override;
};

/**
 * @brief 矩形图形
 */
class RectShape : public Shape {
public:
    Rect rect;
    int32_t layerId_{0};
    
    RectShape() = default;
    RectShape(Rect r, int32_t layer)
        : rect(r), layerId_(layer) {}
    
    ShapeType type() const override { return ShapeType::Rect; }
    int32_t layerId() const override { return layerId_; }
    Rect boundingBox() const override { return rect; }
    std::unique_ptr<Shape> clone() const override;
    
    void translate(int64_t dx, int64_t dy) override {
        rect.translate(dx, dy);
    }
    void rotate(double angle, const Point& center) override;
    void mirror(bool horizontal) override;
};

/**
 * @brief 路径图形
 */
class PathShape : public Shape {
public:
    Path path;
    int32_t layerId_{0};
    
    PathShape() = default;
    PathShape(Path p, int32_t layer)
        : path(std::move(p)), layerId_(layer) {}
    
    ShapeType type() const override { return ShapeType::Path; }
    int32_t layerId() const override { return layerId_; }
    Rect boundingBox() const override;
    std::unique_ptr<Shape> clone() const override;
    
    void translate(int64_t dx, int64_t dy) override;
    void rotate(double angle, const Point& center) override;
    void mirror(bool horizontal) override;
};

/**
 * @brief 文本图形
 */
class TextShape : public Shape {
public:
    Text text;
    int32_t layerId_{0};
    
    TextShape() = default;
    TextShape(Text t, int32_t layer)
        : text(std::move(t)), layerId_(layer) {}
    
    ShapeType type() const override { return ShapeType::Text; }
    int32_t layerId() const override { return layerId_; }
    Rect boundingBox() const override;
    std::unique_ptr<Shape> clone() const override;
    
    void translate(int64_t dx, int64_t dy) override {
        text.position.x += dx;
        text.position.y += dy;
    }
    void rotate(double angle, const Point& center) override;
    void mirror(bool horizontal) override;
};

/**
 * @brief 实例引用图形
 */
class InstanceShape : public Shape {
public:
    CellInstance instance;
    
    InstanceShape() = default;
    explicit InstanceShape(CellInstance inst)
        : instance(std::move(inst)) {}
    
    ShapeType type() const override { return ShapeType::Instance; }
    int32_t layerId() const override { return -1; }  // 实例没有图层
    Rect boundingBox() const override;
    std::unique_ptr<Shape> clone() const override;
    
    void translate(int64_t dx, int64_t dy) override {
        instance.position.x += dx;
        instance.position.y += dy;
    }
    void rotate(double angle, const Point& center) override;
    void mirror(bool horizontal) override;
};

// ============================================================================
// 工具函数
// ============================================================================

/**
 * @brief 计算多边形的边界框
 */
Rect calculateBoundingBox(const std::vector<Point>& points);

/**
 * @brief 旋转点
 */
Point rotatePoint(const Point& p, double angle, const Point& center);

/**
 * @brief 镜像点
 */
Point mirrorPoint(const Point& p, bool horizontal, const Point& center);

} // namespace layout

#endif // LAYOUT_LAYOUT_TYPES_H
