#ifndef DRC_DRC_ENGINE_H
#define DRC_DRC_ENGINE_H

#include "drc/drc_types.h"
#include <QString>
#include <QStringList>
#include <memory>
#include <functional>
#include <chrono>

namespace layout {
    class LayoutDatabase;
}

namespace drc {

/**
 * @brief DRC 引擎
 * 
 * 负责执行设计规则检查
 */
class DRCEngine {
public:
    using ProgressCallback = std::function<void(int current, int total, const QString& ruleName)>;
    
    // ========================================================================
    // 构造与析构
    // ========================================================================
    
    DRCEngine() = default;
    ~DRCEngine() = default;
    
    // ========================================================================
    // 规则管理
    // ========================================================================
    
    /**
     * @brief 获取规则集合
     */
    DRCRuleSet& ruleSet() { return ruleSet_; }
    const DRCRuleSet& ruleSet() const { return ruleSet_; }
    
    /**
     * @brief 添加规则
     */
    void addRule(std::unique_ptr<DRCRule> rule);
    
    /**
     * @brief 加载规则文件
     */
    bool loadRuleFile(const QString& filepath);
    
    /**
     * @brief 保存规则文件
     */
    bool saveRuleFile(const QString& filepath) const;
    
    // ========================================================================
    // DRC 执行
    // ========================================================================
    
    /**
     * @brief 执行 DRC 检查
     * 
     * @param database 版图数据库
     * @return 检查结果
     */
    DRCResult run(const layout::LayoutDatabase& database);
    
    /**
     * @brief 执行特定规则的检查
     */
    DRCResult runRule(const layout::LayoutDatabase& database, const QString& ruleName);
    
    /**
     * @brief 取消当前检查
     */
    void cancel();
    
    /**
     * @brief 检查是否正在运行
     */
    bool isRunning() const { return running_; }
    
    // ========================================================================
    // 回调
    // ========================================================================
    
    /**
     * @brief 设置进度回调
     */
    void setProgressCallback(ProgressCallback callback);
    
    // ========================================================================
    // 统计信息
    // ========================================================================
    
    struct Statistics {
        size_t rulesRun{0};
        size_t totalViolations{0};
        size_t errorCount{0};
        size_t warningCount{0};
        double runTimeMs{0.0};
    };
    
    Statistics lastRunStatistics() const { return lastStats_; }
    
    // ========================================================================
    // 预定义规则
    // ========================================================================
    
    /**
     * @brief 添加最小间距规则
     */
    void addMinSpacingRule(int32_t layer1, int32_t layer2, int64_t minSpace);
    
    /**
     * @brief 添加最小宽度规则
     */
    void addMinWidthRule(int32_t layer, int64_t minWidth);
    
    /**
     * @brief 添加最小包围规则
     */
    void addMinEnclosureRule(int32_t innerLayer, int32_t outerLayer, int64_t minEnclosure);
    
    /**
     * @brief 添加最小面积规则
     */
    void addMinAreaRule(int32_t layer, int64_t minArea);
    
    /**
     * @brief 添加重叠规则
     */
    void addOverlapRule(int32_t layer1, int32_t layer2, bool allowOverlap = false);

private:
    /**
     * @brief 执行单个规则
     */
    std::vector<DRCError> executeRule(const DRCRule& rule, const layout::LayoutDatabase& database);
    
    /**
     * @brief 合并结果
     */
    void mergeResult(DRCResult& result, const std::vector<DRCError>& errors);

private:
    DRCRuleSet ruleSet_;
    ProgressCallback progressCallback_;
    Statistics lastStats_;
    bool running_{false};
    bool cancelled_{false};
};

} // namespace drc

#endif // DRC_DRC_ENGINE_H
