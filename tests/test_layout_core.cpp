#include <gtest/gtest.h>
#include "layout/layout_types.h"
#include "layout/layout_cell.h"
#include "layout/layout_database.h"
#include "drc/drc_types.h"
#include "drc/drc_engine.h"
#include <memory>

// ============================================================================
// Layout Types 测试
// ============================================================================

namespace layout {

class LayoutTypesTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @test 测试 Point 基本功能
 */
TEST_F(LayoutTypesTest, Point_Basic) {
    Point p1;
    EXPECT_EQ(p1.x, 0);
    EXPECT_EQ(p1.y, 0);
    
    Point p2(100, 200);
    EXPECT_EQ(p2.x, 100);
    EXPECT_EQ(p2.y, 200);
}

/**
 * @test 测试 Point 运算符
 */
TEST_F(LayoutTypesTest, Point_Operators) {
    Point p1(100, 200);
    Point p2(50, 30);
    
    Point sum = p1 + p2;
    EXPECT_EQ(sum.x, 150);
    EXPECT_EQ(sum.y, 230);
    
    Point diff = p1 - p2;
    EXPECT_EQ(diff.x, 50);
    EXPECT_EQ(diff.y, 170);
    
    Point scaled = p1 * 2;
    EXPECT_EQ(scaled.x, 200);
    EXPECT_EQ(scaled.y, 400);
}

/**
 * @test 测试 Rect 基本功能
 */
TEST_F(LayoutTypesTest, Rect_Basic) {
    Rect r(0, 0, 100, 200);
    EXPECT_EQ(r.width(), 100);
    EXPECT_EQ(r.height(), 200);
    EXPECT_EQ(r.area(), 20000);
    
    Point center = r.center();
    EXPECT_EQ(center.x, 50);
    EXPECT_EQ(center.y, 100);
}

/**
 * @test 测试 Rect 包含检查
 */
TEST_F(LayoutTypesTest, Rect_Contains) {
    Rect r(0, 0, 100, 100);
    
    EXPECT_TRUE(r.contains(Point(50, 50)));
    EXPECT_TRUE(r.contains(Point(0, 0)));
    EXPECT_TRUE(r.contains(Point(100, 100)));
    EXPECT_FALSE(r.contains(Point(-1, 50)));
    EXPECT_FALSE(r.contains(Point(101, 50)));
}

/**
 * @test 测试 Rect 相交检查
 */
TEST_F(LayoutTypesTest, Rect_Intersects) {
    Rect r1(0, 0, 100, 100);
    Rect r2(50, 50, 150, 150);
    Rect r3(200, 200, 300, 300);
    
    EXPECT_TRUE(r1.intersects(r2));
    EXPECT_FALSE(r1.intersects(r3));
}

/**
 * @test 测试 Rect 相交计算
 */
TEST_F(LayoutTypesTest, Rect_Intersect) {
    Rect r1(0, 0, 100, 100);
    Rect r2(50, 50, 150, 150);
    
    Rect intersection = r1.intersect(r2);
    EXPECT_EQ(intersection.left, 50);
    EXPECT_EQ(intersection.bottom, 50);
    EXPECT_EQ(intersection.right, 100);
    EXPECT_EQ(intersection.top, 100);
}

/**
 * @test 测试 Rect 平移
 */
TEST_F(LayoutTypesTest, Rect_Translate) {
    Rect r(0, 0, 100, 100);
    r.translate(10, 20);
    
    EXPECT_EQ(r.left, 10);
    EXPECT_EQ(r.bottom, 20);
    EXPECT_EQ(r.right, 110);
    EXPECT_EQ(r.top, 120);
}

/**
 * @test 测试 Polygon 基本功能
 */
TEST_F(LayoutTypesTest, Polygon_Basic) {
    Polygon poly;
    poly.addPoint(0, 0);
    poly.addPoint(100, 0);
    poly.addPoint(100, 100);
    poly.addPoint(0, 100);
    
    EXPECT_EQ(poly.pointCount(), 4);
    EXPECT_TRUE(poly.isValid());
    
    Rect box = poly.boundingBox();
    EXPECT_EQ(box.left, 0);
    EXPECT_EQ(box.bottom, 0);
    EXPECT_EQ(box.right, 100);
    EXPECT_EQ(box.top, 100);
}

/**
 * @test 测试 CellInstance 变换
 */
TEST_F(LayoutTypesTest, CellInstance_Transform) {
    CellInstance inst("CELL1", Point(100, 200));
    
    Point p(10, 20);
    Point transformed = inst.transform(p);
    
    // 无旋转无镜像，只平移
    EXPECT_EQ(transformed.x, 110);
    EXPECT_EQ(transformed.y, 220);
}

/**
 * @test 测试 CellInstance 镜像变换
 */
TEST_F(LayoutTypesTest, CellInstance_Mirror) {
    CellInstance inst("CELL1", Point(0, 0));
    inst.mirrorX = true;
    
    Point p(10, 20);
    Point transformed = inst.transform(p);
    
    EXPECT_EQ(transformed.x, -10);
    EXPECT_EQ(transformed.y, 20);
}

/**
 * @test 测试 Shape 创建
 */
TEST_F(LayoutTypesTest, Shape_Creation) {
    // 多边形
    Polygon poly;
    poly.addPoint(0, 0);
    poly.addPoint(100, 0);
    poly.addPoint(100, 100);
    
    PolygonShape polyShape(poly, 1);
    EXPECT_EQ(polyShape.type(), ShapeType::Polygon);
    EXPECT_EQ(polyShape.layerId(), 1);
    
    // 矩形
    RectShape rectShape(Rect(0, 0, 50, 50), 2);
    EXPECT_EQ(rectShape.type(), ShapeType::Rect);
    EXPECT_EQ(rectShape.layerId(), 2);
    
    // 路径
    Path path;
    path.addPoint(0, 0);
    path.addPoint(100, 100);
    PathShape pathShape(path, 3);
    EXPECT_EQ(pathShape.type(), ShapeType::Path);
    
    // 文本
    Text text("Label", Point(50, 50), 10);
    TextShape textShape(text, 4);
    EXPECT_EQ(textShape.type(), ShapeType::Text);
}

/**
 * @test 测试 Shape 克隆
 */
TEST_F(LayoutTypesTest, Shape_Clone) {
    RectShape original(Rect(0, 0, 100, 100), 1);
    
    auto cloned = original.clone();
    EXPECT_EQ(cloned->type(), ShapeType::Rect);
    EXPECT_EQ(cloned->layerId(), 1);
    EXPECT_EQ(cloned->boundingBox(), original.boundingBox());
}

/**
 * @test 测试 Shape 平移
 */
TEST_F(LayoutTypesTest, Shape_Translate) {
    RectShape shape(Rect(0, 0, 100, 100), 1);
    shape.translate(10, 20);
    
    Rect box = shape.boundingBox();
    EXPECT_EQ(box.left, 10);
    EXPECT_EQ(box.bottom, 20);
}

} // namespace layout

// ============================================================================
// LayoutCell 测试
// ============================================================================

namespace layout {

class LayoutCellTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @test 测试 LayoutCell 构造
 */
TEST_F(LayoutCellTest, Constructor) {
    LayoutCell cell("TOP");
    EXPECT_EQ(cell.name(), "TOP");
    EXPECT_TRUE(cell.isEmpty());
    EXPECT_EQ(cell.shapeCount(), 0);
}

/**
 * @test 测试 LayoutCell 添加图形
 */
TEST_F(LayoutCellTest, AddShapes) {
    LayoutCell cell("TOP");
    
    // 添加矩形
    cell.addRect(Rect(0, 0, 100, 100), 1);
    EXPECT_EQ(cell.shapeCount(), 1);
    
    // 添加多边形
    Polygon poly;
    poly.addPoint(0, 0);
    poly.addPoint(50, 0);
    poly.addPoint(50, 50);
    cell.addPolygon(std::move(poly), 2);
    EXPECT_EQ(cell.shapeCount(), 2);
    
    // 添加路径
    Path path;
    path.addPoint(0, 0);
    path.addPoint(100, 100);
    cell.addPath(std::move(path), 1);
    EXPECT_EQ(cell.shapeCount(), 3);
}

/**
 * @test 测试 LayoutCell 添加实例
 */
TEST_F(LayoutCellTest, AddInstance) {
    LayoutCell cell("TOP");
    
    CellInstance inst("SUBCELL", Point(100, 200));
    cell.addInstance(inst);
    
    EXPECT_EQ(cell.instanceCount(), 1);
    auto refs = cell.referencedCells();
    EXPECT_EQ(refs.size(), 1);
    EXPECT_EQ(refs[0], "SUBCELL");
}

/**
 * @test 测试 LayoutCell 图层管理
 */
TEST_F(LayoutCellTest, LayerManagement) {
    LayoutCell cell("TOP");
    
    cell.addRect(Rect(0, 0, 100, 100), 1);
    cell.addRect(Rect(50, 50, 150, 150), 2);
    
    EXPECT_TRUE(cell.hasLayer(1));
    EXPECT_TRUE(cell.hasLayer(2));
    EXPECT_FALSE(cell.hasLayer(3));
    
    auto layerIds = cell.layerIds();
    EXPECT_EQ(layerIds.size(), 2);
    
    auto layer = cell.getLayer(1);
    EXPECT_NE(layer, nullptr);
    EXPECT_EQ(layer->id, 1);
}

/**
 * @test 测试 LayoutCell 边界框
 */
TEST_F(LayoutCellTest, BoundingBox) {
    LayoutCell cell("TOP");
    
    cell.addRect(Rect(0, 0, 100, 100), 1);
    cell.addRect(Rect(50, 50, 200, 200), 1);
    
    Rect box = cell.boundingBox();
    EXPECT_EQ(box.left, 0);
    EXPECT_EQ(box.bottom, 0);
    EXPECT_EQ(box.right, 200);
    EXPECT_EQ(box.top, 200);
}

/**
 * @test 测试 LayoutCell 平移
 */
TEST_F(LayoutCellTest, Translate) {
    LayoutCell cell("TOP");
    cell.addRect(Rect(0, 0, 100, 100), 1);
    
    cell.translate(10, 20);
    
    Rect box = cell.boundingBox();
    EXPECT_EQ(box.left, 10);
    EXPECT_EQ(box.bottom, 20);
}

/**
 * @test 测试 LayoutCell 克隆
 */
TEST_F(LayoutCellTest, Clone) {
    LayoutCell original("TOP");
    original.addRect(Rect(0, 0, 100, 100), 1);
    original.addRect(Rect(50, 50, 150, 150), 2);
    
    auto cloned = original.clone();
    
    EXPECT_EQ(cloned->name(), "TOP");
    EXPECT_EQ(cloned->shapeCount(), 2);
    EXPECT_EQ(cloned->boundingBox(), original.boundingBox());
}

} // namespace layout

// ============================================================================
// LayoutDatabase 测试
// ============================================================================

namespace layout {

class LayoutDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @test 测试 LayoutDatabase 构造
 */
TEST_F(LayoutDatabaseTest, Constructor) {
    LayoutDatabase db;
    EXPECT_TRUE(db.libName().empty());
    EXPECT_TRUE(db.topCellName().empty());
    EXPECT_EQ(db.cellCount(), 0);
}

/**
 * @test 测试 LayoutDatabase 创建单元
 */
TEST_F(LayoutDatabaseTest, CreateCell) {
    LayoutDatabase db;
    
    auto& cell1 = db.createCell("TOP");
    EXPECT_EQ(cell1.name(), "TOP");
    
    auto& cell2 = db.createCell("SUB1");
    EXPECT_EQ(cell2.name(), "SUB1");
    
    EXPECT_EQ(db.cellCount(), 2);
}

/**
 * @test 测试 LayoutDatabase 获取单元
 */
TEST_F(LayoutDatabaseTest, GetCell) {
    LayoutDatabase db;
    db.createCell("TOP");
    
    auto* cell = db.getCell("TOP");
    EXPECT_NE(cell, nullptr);
    EXPECT_EQ(cell->name(), "TOP");
    
    auto* notFound = db.getCell("NOTEXIST");
    EXPECT_EQ(notFound, nullptr);
}

/**
 * @test 测试 LayoutDatabase 顶层单元
 */
TEST_F(LayoutDatabaseTest, TopCell) {
    LayoutDatabase db;
    db.setTopCellName("TOP");
    db.createCell("TOP");
    
    auto* top = db.topCell();
    EXPECT_NE(top, nullptr);
    EXPECT_EQ(top->name(), "TOP");
}

/**
 * @test 测试 LayoutDatabase 单元引用检查
 */
TEST_F(LayoutDatabaseTest, CellReferences) {
    LayoutDatabase db;
    
    auto& top = db.createCell("TOP");
    top.addInstance(CellInstance("SUB1", Point(0, 0)));
    top.addInstance(CellInstance("SUB2", Point(100, 100)));
    
    db.createCell("SUB1");
    db.createCell("SUB2");
    
    auto refs = db.getCellReferences("SUB1");
    EXPECT_EQ(refs.size(), 1);
    EXPECT_EQ(refs[0], "TOP");
}

/**
 * @test 测试 LayoutDatabase 循环引用检查
 */
TEST_F(LayoutDatabaseTest, CircularReference) {
    LayoutDatabase db;
    
    auto& cellA = db.createCell("CELL_A");
    auto& cellB = db.createCell("CELL_B");
    
    // A 引用 B
    cellA.addInstance(CellInstance("CELL_B", Point(0, 0)));
    
    // B 引用 A (循环)
    cellB.addInstance(CellInstance("CELL_A", Point(0, 0)));
    
    EXPECT_TRUE(db.hasCircularReference("CELL_A"));
    EXPECT_TRUE(db.hasCircularReference("CELL_B"));
}

/**
 * @test 测试 LayoutDatabase 层次化统计
 */
TEST_F(LayoutDatabaseTest, HierarchicalCount) {
    LayoutDatabase db;
    
    auto& top = db.createCell("TOP");
    top.addRect(Rect(0, 0, 100, 100), 1);
    top.addInstance(CellInstance("SUB1", Point(0, 0)));
    
    auto& sub1 = db.createCell("SUB1");
    sub1.addRect(Rect(0, 0, 50, 50), 1);
    sub1.addInstance(CellInstance("SUB2", Point(0, 0)));
    
    auto& sub2 = db.createCell("SUB2");
    sub2.addRect(Rect(0, 0, 25, 25), 1);
    
    size_t count = db.hierarchicalShapeCount("TOP");
    // TOP: 1 rect + 1 instance
    // SUB1: 1 rect + 1 instance
    // SUB2: 1 rect
    EXPECT_GE(count, 3);  // 至少 3 个图形
}

/**
 * @test 测试 LayoutDatabase 统计信息
 */
TEST_F(LayoutDatabaseTest, Statistics) {
    LayoutDatabase db;
    db.setLibName("TestLib");
    
    auto& top = db.createCell("TOP");
    top.addRect(Rect(0, 0, 100, 100), 1);
    top.addRect(Rect(50, 50, 150, 150), 2);
    
    auto stats = db.getStatistics();
    EXPECT_EQ(stats.cellCount, 1);
    EXPECT_EQ(stats.totalShapeCount, 2);
    EXPECT_EQ(stats.uniqueLayerCount, 2);
}

} // namespace layout

// ============================================================================
// DRC 测试
// ============================================================================

namespace drc {

class DRCTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @test 测试 DRC 规则创建
 */
TEST_F(DRCTest, RuleCreation) {
    MinSpacingRule spacingRule(1, 2, 100);
    EXPECT_EQ(spacingRule.name(), "MIN_SPACING_L1_L2");
    EXPECT_TRUE(spacingRule.isEnabled());
    
    MinWidthRule widthRule(1, 50);
    EXPECT_EQ(widthRule.name(), "MIN_WIDTH_L1");
    
    MinEnclosureRule encRule(1, 2, 20);
    EXPECT_EQ(encRule.name(), "MIN_ENCLOSURE_L1_BY_L2");
    
    MinAreaRule areaRule(1, 1000);
    EXPECT_EQ(areaRule.name(), "MIN_AREA_L1");
    
    OverlapRule overlapRule(1, 2, false);
    EXPECT_EQ(overlapRule.name(), "NO_OVERLAP_L1_L2");
}

/**
 * @test 测试 DRC 规则集合
 */
TEST_F(DRCTest, RuleSet) {
    DRCRuleSet ruleSet;
    
    ruleSet.addRule(std::make_unique<MinSpacingRule>(1, 2, 100));
    ruleSet.addRule(std::make_unique<MinWidthRule>(1, 50));
    
    EXPECT_EQ(ruleSet.ruleCount(), 2);
    
    auto* rule = ruleSet.getRule("MIN_SPACING_L1_L2");
    EXPECT_NE(rule, nullptr);
    
    ruleSet.setAllRulesEnabled(false);
    EXPECT_FALSE(ruleSet.getRule("MIN_SPACING_L1_L2")->isEnabled());
}

/**
 * @test 测试 DRC 引擎添加规则
 */
TEST_F(DRCTest, Engine_AddRules) {
    DRCEngine engine;
    
    engine.addMinSpacingRule(1, 2, 100);
    engine.addMinWidthRule(1, 50);
    engine.addMinEnclosureRule(1, 2, 20);
    engine.addMinAreaRule(1, 1000);
    engine.addOverlapRule(1, 2, false);
    
    const auto& ruleSet = engine.ruleSet();
    EXPECT_EQ(ruleSet.ruleCount(), 5);
}

/**
 * @test 测试 DRC 结果
 */
TEST_F(DRCTest, DRCResult) {
    DRCResult result;
    
    result.errors.push_back(DRCError(DRCErrorType::MinSpacing, DRCSeverity::Error, 
        "RULE1", "Spacing violation"));
    result.warnings.push_back(DRCError(DRCErrorType::MinWidth, DRCSeverity::Warning,
        "RULE2", "Width warning"));
    
    EXPECT_TRUE(result.hasErrors());
    EXPECT_TRUE(result.hasWarnings());
    EXPECT_EQ(result.totalViolations(), 2);
}

/**
 * @test 测试 DRC 引擎运行 (空数据库)
 */
TEST_F(DRCTest, Engine_RunEmpty) {
    DRCEngine engine;
    engine.addMinSpacingRule(1, 2, 100);
    
    layout::LayoutDatabase db;
    DRCResult result = engine.run(db);
    
    EXPECT_TRUE(result.completed);
    EXPECT_EQ(result.errors.size(), 0);  // 空数据库无违规
}

} // namespace drc

// ============================================================================
// 集成测试
// ============================================================================

namespace layout {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @test 测试完整的版图数据库创建
 */
TEST_F(IntegrationTest, CreateCompleteLayout) {
    LayoutDatabase db;
    db.setLibName("TestChip");
    db.setTopCellName("TOP");
    
    // 创建顶层单元
    auto& top = db.createCell("TOP");
    top.addRect(Rect(0, 0, 1000, 1000), 1);  // 有源区
    
    // 创建子单元
    auto& subcell = db.createCell("SUBCELL");
    subcell.addRect(Rect(0, 0, 100, 100), 1);
    subcell.addPolygon([](){
        Polygon p;
        p.addPoint(0, 0);
        p.addPoint(50, 0);
        p.addPoint(50, 50);
        p.addPoint(0, 50);
        return p;
    }(), 2);
    
    // 在顶层实例化子单元
    top.addInstance(CellInstance("SUBCELL", Point(200, 200)));
    top.addInstance(CellInstance("SUBCELL", Point(400, 200)));
    
    // 验证
    EXPECT_EQ(db.cellCount(), 2);
    EXPECT_EQ(db.topCell()->shapeCount(), 2);  // 1 rect + 1 instance
    EXPECT_EQ(subcell.shapeCount(), 2);  // 1 rect + 1 polygon
    
    // 验证层次化统计
    size_t hierCount = db.hierarchicalShapeCount("TOP");
    EXPECT_GE(hierCount, 3);
}

/**
 * @test 测试版图变换
 */
TEST_F(IntegrationTest, LayoutTransform) {
    LayoutCell cell("TEST");
    cell.addRect(Rect(0, 0, 100, 100), 1);
    cell.addRect(Rect(200, 200, 300, 300), 1);
    
    // 平移
    cell.translate(50, 50);
    Rect box1 = cell.boundingBox();
    EXPECT_EQ(box1.left, 50);
    EXPECT_EQ(box1.bottom, 50);
    
    // 旋转 90 度
    cell.rotate(90.0, Point(0, 0));
    
    // 镜像
    cell.mirror(true);
}

} // namespace layout
