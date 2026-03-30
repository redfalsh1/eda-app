#include "drc/drc_engine.h"
#include "layout/layout_database.h"
#include <algorithm>
#include <chrono>

namespace drc {

// ============================================================================
// DRCRuleSet 实现
// ============================================================================

void DRCRuleSet::addRule(RulePtr rule) {
    if (rule) {
        rules_.push_back(std::move(rule));
    }
}

bool DRCRuleSet::removeRule(const QString& name) {
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&name](const RulePtr& rule) {
            return rule->name() == name;
        });
    
    if (it != rules_.end()) {
        rules_.erase(it);
        return true;
    }
    return false;
}

DRCRule* DRCRuleSet::getRule(const QString& name) {
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&name](const RulePtr& rule) {
            return rule->name() == name;
        });
    
    return (it != rules_.end()) ? it->get() : nullptr;
}

const DRCRule* DRCRuleSet::getRule(const QString& name) const {
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&name](const RulePtr& rule) {
            return rule->name() == name;
        });
    
    return (it != rules_.end()) ? it->get() : nullptr;
}

void DRCRuleSet::setAllRulesEnabled(bool enabled) {
    for (auto& rule : rules_) {
        rule->setEnabled(enabled);
    }
}

DRCRuleSet::RuleList DRCRuleSet::enabledRules() const {
    RuleList result;
    for (const auto& rule : rules_) {
        if (rule->isEnabled()) {
            // 需要克隆，因为返回的是 unique_ptr
            // 这里简化处理，实际应用中可能需要不同的设计
        }
    }
    return result;
}

// ============================================================================
// DRCEngine 实现
// ============================================================================

void DRCEngine::addRule(std::unique_ptr<DRCRule> rule) {
    ruleSet_.addRule(std::move(rule));
}

bool DRCEngine::loadRuleFile(const QString& filepath) {
    // TODO: 实现规则文件加载 (JSON/XML 格式)
    return false;
}

bool DRCEngine::saveRuleFile(const QString& filepath) const {
    // TODO: 实现规则文件保存
    return false;
}

DRCResult DRCEngine::run(const layout::LayoutDatabase& database) {
    running_ = true;
    cancelled_ = false;
    
    DRCResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    size_t totalRules = ruleSet_.rules().size();
    size_t currentRule = 0;
    
    for (const auto& rule : ruleSet_.rules()) {
        if (cancelled_) {
            break;
        }
        
        if (!rule->isEnabled()) {
            continue;
        }
        
        currentRule++;
        
        // 进度回调
        if (progressCallback_) {
            progressCallback_(static_cast<int>(currentRule), static_cast<int>(totalRules), rule->name());
        }
        
        // 执行规则
        auto errors = executeRule(*rule, database);
        mergeResult(result, errors);
        
        lastStats_.rulesRun++;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.runTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.completed = !cancelled_;
    
    lastStats_.totalViolations = result.totalViolations();
    lastStats_.errorCount = result.errors.size();
    lastStats_.warningCount = result.warnings.size();
    lastStats_.runTimeMs = result.runTimeMs;
    
    running_ = false;
    return result;
}

DRCResult DRCEngine::runRule(const layout::LayoutDatabase& database, const QString& ruleName) {
    DRCResult result;
    
    const DRCRule* rule = ruleSet_.getRule(ruleName);
    if (!rule) {
        result.errors.push_back(DRCError(
            DRCErrorType::Other,
            DRCSeverity::Error,
            "RuleNotFound",
            QString("Rule not found: %1").arg(ruleName)
        ));
        return result;
    }
    
    if (!rule->isEnabled()) {
        result.errors.push_back(DRCError(
            DRCErrorType::Other,
            DRCSeverity::Warning,
            "RuleDisabled",
            QString("Rule is disabled: %1").arg(ruleName)
        ));
    }
    
    auto errors = executeRule(*rule, database);
    mergeResult(result, errors);
    
    result.completed = true;
    return result;
}

void DRCEngine::cancel() {
    cancelled_ = true;
}

void DRCEngine::setProgressCallback(ProgressCallback callback) {
    progressCallback_ = std::move(callback);
}

void DRCEngine::addMinSpacingRule(int32_t layer1, int32_t layer2, int64_t minSpace) {
    auto rule = std::make_unique<MinSpacingRule>(layer1, layer2, minSpace);
    addRule(std::move(rule));
}

void DRCEngine::addMinWidthRule(int32_t layer, int64_t minWidth) {
    auto rule = std::make_unique<MinWidthRule>(layer, minWidth);
    addRule(std::move(rule));
}

void DRCEngine::addMinEnclosureRule(int32_t innerLayer, int32_t outerLayer, int64_t minEnclosure) {
    auto rule = std::make_unique<MinEnclosureRule>(innerLayer, outerLayer, minEnclosure);
    addRule(std::move(rule));
}

void DRCEngine::addMinAreaRule(int32_t layer, int64_t minArea) {
    auto rule = std::make_unique<MinAreaRule>(layer, minArea);
    addRule(std::move(rule));
}

void DRCEngine::addOverlapRule(int32_t layer1, int32_t layer2, bool allowOverlap) {
    auto rule = std::make_unique<OverlapRule>(layer1, layer2, allowOverlap);
    addRule(std::move(rule));
}

std::vector<DRCError> DRCEngine::executeRule(const DRCRule& rule, const layout::LayoutDatabase& database) {
    // 调用规则的 check 方法
    return rule.check(&database);
}

void DRCEngine::mergeResult(DRCResult& result, const std::vector<DRCError>& errors) {
    for (const auto& error : errors) {
        switch (error.severity) {
            case DRCSeverity::Error:
                result.errors.push_back(error);
                break;
            case DRCSeverity::Warning:
                result.warnings.push_back(error);
                break;
            case DRCSeverity::Info:
                result.infos.push_back(error);
                break;
        }
    }
}

} // namespace drc
