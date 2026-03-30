#ifndef DRC_DRC_TYPES_H
#define DRC_DRC_TYPES_H

#include "layout/layout_types.h"
#include <QString>
#include <vector>
#include <memory>
#include <functional>

namespace drc {

// ============================================================================
// 错误类型
// ============================================================================

/**
 * @brief DRC 错误类型
 */
enum class DRCErrorType {
    MinSpacing,      ///< 最小间距违规
    MinWidth,        ///< 最小宽度违规
    MinEnclosure,    ///< 最小包围违规
    MinExtension,    ///< 最小延伸违规
    MinArea,         ///< 最小面积违规
    MinHole,         ///< 最小孔洞违规
    Overlap,         ///< 重叠违规
    Other            ///< 其他违规
};

/**
 * @brief DRC 错误严重性
 */
enum class DRCSeverity {
    Error,    ///< 错误 (必须修复)
    Warning,  ///< 警告 (建议修复)
    Info      ///< 提示 (仅供参考)
};

/**
 * @brief DRC 错误
 */
struct DRCError {
    DRCErrorType type;
    DRCSeverity severity;
    QString ruleName;
    QString message;
    
    // 错误位置
    layout::Rect boundingBox;
    std::vector<layout::Point> markers;
    
    // 相关信息
    int32_t layerId1{-1};
    int32_t layerId2{-1};
    int64_t expectedValue{0};
    int64_t actualValue{0};
    
    DRCError() = default;
    
    DRCError(DRCErrorType t, DRCSeverity s, const QString& rule, const QString& msg)
        : type(t), severity(s), ruleName(rule), message(msg) {}
};

// ============================================================================
// 规则定义
// ============================================================================

/**
 * @brief 规则类型
 */
enum class DRCRuleType {
    MinSpacing,    ///< 最小间距
    MinWidth,      ///< 最小宽度
    MinEnclosure,  ///< 最小包围
    MinExtension,  ///< 最小延伸
    MinArea,       ///< 最小面积
    MinHole,       ///< 最小孔洞
    MaxDensity,    ///< 最大密度
    Overlap,       ///< 重叠检查
    Custom         ///< 自定义规则
};

/**
 * @brief DRC 规则基类
 */
class DRCRule {
public:
    virtual ~DRCRule() = default;
    
    /**
     * @brief 规则名称
     */
    virtual QString name() const = 0;
    
    /**
     * @brief 规则描述
     */
    virtual QString description() const { return QString(); }
    
    /**
     * @brief 规则类型
     */
    virtual DRCRuleType type() const = 0;
    
    /**
     * @brief 检查违规
     * 
     * @param database 版图数据库
     * @return 错误列表
     */
    virtual std::vector<DRCError> check(const void* database) const = 0;
    
    /**
     * @brief 规则是否启用
     */
    virtual bool isEnabled() const { return enabled_; }
    
    /**
     * @brief 设置启用状态
     */
    virtual void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * @brief 规则严重性
     */
    virtual DRCSeverity severity() const { return severity_; }
    
    /**
     * @brief 设置严重性
     */
    virtual void setSeverity(DRCSeverity severity) { severity_ = severity; }

protected:
    bool enabled_{true};
    DRCSeverity severity_{DRCSeverity::Error};
};

// ============================================================================
// 具体规则类
// ============================================================================

/**
 * @brief 最小间距规则
 */
class MinSpacingRule : public DRCRule {
public:
    MinSpacingRule() = default;
    
    MinSpacingRule(int32_t layer1, int32_t layer2, int64_t minSpace)
        : layer1_(layer1), layer2_(layer2), minSpace_(minSpace) {}
    
    QString name() const override {
        return QString("MIN_SPACING_L%1_L%2").arg(layer1_).arg(layer2_);
    }
    
    QString description() const override {
        return QString("Minimum spacing between layer %1 and %2: %3 nm")
            .arg(layer1_).arg(layer2_).arg(minSpace_);
    }
    
    DRCRuleType type() const override { return DRCRuleType::MinSpacing; }
    
    std::vector<DRCError> check(const void* database) const override;
    
    // 参数
    int32_t layer1_{0};
    int32_t layer2_{0};
    int64_t minSpace_{0};
};

/**
 * @brief 最小宽度规则
 */
class MinWidthRule : public DRCRule {
public:
    MinWidthRule() = default;
    
    MinWidthRule(int32_t layer, int64_t minWidth)
        : layer_(layer), minWidth_(minWidth) {}
    
    QString name() const override {
        return QString("MIN_WIDTH_L%1").arg(layer_);
    }
    
    QString description() const override {
        return QString("Minimum width on layer %1: %2 nm").arg(layer_).arg(minWidth_);
    }
    
    DRCRuleType type() const override { return DRCRuleType::MinWidth; }
    
    std::vector<DRCError> check(const void* database) const override;
    
    // 参数
    int32_t layer_{0};
    int64_t minWidth_{0};
};

/**
 * @brief 最小包围规则
 */
class MinEnclosureRule : public DRCRule {
public:
    MinEnclosureRule() = default;
    
    MinEnclosureRule(int32_t innerLayer, int32_t outerLayer, int64_t minEnclosure)
        : innerLayer_(innerLayer), outerLayer_(outerLayer), minEnclosure_(minEnclosure) {}
    
    QString name() const override {
        return QString("MIN_ENCLOSURE_L%1_BY_L%2").arg(innerLayer_).arg(outerLayer_);
    }
    
    QString description() const override {
        return QString("Layer %1 must be enclosed by layer %2 by at least %3 nm")
            .arg(innerLayer_).arg(outerLayer_).arg(minEnclosure_);
    }
    
    DRCRuleType type() const override { return DRCRuleType::MinEnclosure; }
    
    std::vector<DRCError> check(const void* database) const override;
    
    // 参数
    int32_t innerLayer_{0};
    int32_t outerLayer_{0};
    int64_t minEnclosure_{0};
};

/**
 * @brief 最小面积规则
 */
class MinAreaRule : public DRCRule {
public:
    MinAreaRule() = default;
    
    MinAreaRule(int32_t layer, int64_t minArea)
        : layer_(layer), minArea_(minArea) {}
    
    QString name() const override {
        return QString("MIN_AREA_L%1").arg(layer_);
    }
    
    QString description() const override {
        return QString("Minimum area on layer %1: %2 sq.nm").arg(layer_).arg(minArea_);
    }
    
    DRCRuleType type() const override { return DRCRuleType::MinArea; }
    
    std::vector<DRCError> check(const void* database) const override;
    
    // 参数
    int32_t layer_{0};
    int64_t minArea_{0};
};

/**
 * @brief 重叠规则
 */
class OverlapRule : public DRCRule {
public:
    OverlapRule() = default;
    
    OverlapRule(int32_t layer1, int32_t layer2, bool allowOverlap = false)
        : layer1_(layer1), layer2_(layer2), allowOverlap_(allowOverlap) {}
    
    QString name() const override {
        return allowOverlap_ ? 
            QString("MUST_OVERLAP_L%1_L%2").arg(layer1_).arg(layer2_) :
            QString("NO_OVERLAP_L%1_L%2").arg(layer1_).arg(layer2_);
    }
    
    QString description() const override {
        return allowOverlap_ ?
            QString("Layer %1 must overlap with layer %2").arg(layer1_).arg(layer2_) :
            QString("Layer %1 must not overlap with layer %2").arg(layer1_).arg(layer2_);
    }
    
    DRCRuleType type() const override { return DRCRuleType::Overlap; }
    
    std::vector<DRCError> check(const void* database) const override;
    
    // 参数
    int32_t layer1_{0};
    int32_t layer2_{0};
    bool allowOverlap_{false};  // false=不允许重叠，true=必须重叠
};

// ============================================================================
// 规则管理器
// ============================================================================

/**
 * @brief DRC 规则集合
 */
class DRCRuleSet {
public:
    using RulePtr = std::unique_ptr<DRCRule>;
    using RuleList = std::vector<RulePtr>;
    
    /**
     * @brief 添加规则
     */
    void addRule(RulePtr rule);
    
    /**
     * @brief 移除规则
     */
    bool removeRule(const QString& name);
    
    /**
     * @brief 获取规则
     */
    DRCRule* getRule(const QString& name);
    const DRCRule* getRule(const QString& name) const;
    
    /**
     * @brief 获取所有规则
     */
    const RuleList& rules() const { return rules_; }
    
    /**
     * @brief 获取规则数量
     */
    size_t ruleCount() const { return rules_.size(); }
    
    /**
     * @brief 启用/禁用所有规则
     */
    void setAllRulesEnabled(bool enabled);
    
    /**
     * @brief 获取启用的规则
     */
    RuleList enabledRules() const;

private:
    RuleList rules_;
};

// ============================================================================
// DRC 结果
// ============================================================================

/**
 * @brief DRC 检查结果
 */
struct DRCResult {
    std::vector<DRCError> errors;
    std::vector<DRCError> warnings;
    std::vector<DRCError> infos;
    
    double runTimeMs{0.0};
    bool completed{false};
    
    size_t totalViolations() const {
        return errors.size() + warnings.size() + infos.size();
    }
    
    bool hasErrors() const {
        return !errors.empty();
    }
    
    bool hasWarnings() const {
        return !warnings.empty();
    }
};

} // namespace drc

#endif // DRC_DRC_TYPES_H
