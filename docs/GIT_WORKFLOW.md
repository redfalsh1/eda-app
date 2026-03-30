# Git 开发规范 - 统一账号 + 角色标签

**版本**: v1.0  
**最后更新**: 2026-03-30  
**维护**: eda_tech_lead

---

## 📋 统一账号配置

### GitHub 账号
- **用户名**: `redfalsh1`
- **邮箱**: `redfalsh1@users.noreply.github.com`
- **显示名**: `EDA Team`

### 本地配置命令
```bash
# 全局配置（首次执行）
git config --global user.name "EDA Team"
git config --global user.email "redfalsh1@users.noreply.github.com"

# 或者项目级配置（推荐）
cd eda-app
git config user.name "EDA Team"
git config user.email "redfalsh1@users.noreply.github.com"
```

### 验证配置
```bash
git config user.name
git config user.email
```

---

## 🏷️ 角色标签规范

### 提交信息格式
```
[角色标签] 类型：描述

详细说明（可选）
```

### 角色标签定义

| 标签 | 角色 | 负责人 | 说明 |
|------|------|--------|------|
| `[TL]` | Tech Lead | eda_tech_lead | 技术决策、架构设计、代码审查 |
| `[CPP]` | C++ Developer | eda_cpp_dev | C++ 核心逻辑、算法、数据结构 |
| `[QT]` | Qt Developer | eda_qt_dev | Qt UI、界面、交互、控件 |
| `[QA]` | QA Engineer | eda_qa | 测试用例、自动化测试、Bug 验证 |
| `[OPS]` | DevOps Engineer | eda_devops | CI/CD、构建、部署、文档 |

### 提交类型

| 类型 | 说明 | 示例 |
|------|------|------|
| `feat` | 新功能 | `[CPP] feat: 添加 GDSII 解析器` |
| `fix` | Bug 修复 | `[QT] fix: 修复画布缩放抖动` |
| `docs` | 文档更新 | `[TL] docs: 更新架构设计文档` |
| `style` | 代码格式 | `[CPP] style: 格式化几何引擎代码` |
| `refactor` | 重构 | `[QT] refactor: 重构主窗口布局` |
| `test` | 测试相关 | `[QA] test: 添加 DRC 引擎测试` |
| `chore` | 构建/工具 | `[OPS] chore: 更新 CI 配置` |
| `merge` | 合并分支 | `[TL] merge: 合并 feature/ui 到 dev` |

---

## 📝 提交信息示例

### 新功能
```bash
git commit -m "[CPP] feat: 实现插件系统框架

- 添加 IPlugin 接口定义
- 实现 PluginManager 单例
- 支持动态库加载/卸载
- 添加插件依赖管理"
```

### Bug 修复
```bash
git commit -m "[QT] fix: 修复画布高 DPI 缩放问题

- 修复 Qt 高 DPI 适配
- 调整画布渲染比例
- 添加 DPI 感知测试"
```

### 文档更新
```bash
git commit -m "[OPS] docs: 添加开发环境配置指南

- Windows 安装步骤
- Linux 安装步骤
- IDE 配置说明
- 常见问题解答"
```

### 合并提交
```bash
git commit -m "[TL] merge: 合并 feature/layout-editor 到 dev

- 合并 UI 框架代码
- 解决 CMakeLists.txt 冲突
- 更新构建配置"
```

---

## 🔧 Git 别名配置（可选）

### 快捷配置
```bash
# 添加角色标签别名
git config --global alias.commit-tl "!f() { git commit -m \"[TL] $*\"; }; f"
git config --global alias.commit-cpp "!f() { git commit -m \"[CPP] $*\"; }; f"
git config --global alias.commit-qt "!f() { git commit -m \"[QT] $*\"; }; f"
git config --global alias.commit-qa "!f() { git commit -m \"[QA] $*\"; }; f"
git config --global alias.commit-ops "!f() { git commit -m \"[OPS] $*\"; }; f"

# 使用示例
git commit-cpp "feat: 添加几何引擎"
git commit-qt "fix: 修复图层面板崩溃"
```

### 查看带标签的日志
```bash
# 查看某角色的提交
git log --grep="\[CPP\]" --oneline
git log --grep="\[QT\]" --oneline

# 查看某类型的提交
git log --grep="feat:" --oneline
git log --grep="fix:" --oneline
```

---

## 📊 分支策略

### 分支结构
```
main (稳定分支)
  └─ 只有 release 时合并
  └─ 受保护，需要 PR + CI 通过

dev (开发分支)
  └─ 日常开发在此分支
  └─ 直接推送（需 pull 最新）

feature/* (可选功能分支)
  └─ 大功能开发
  └─ 完成后合并到 dev
```

### 开发流程
```bash
# 1. 拉取最新代码
git checkout dev
git pull origin dev

# 2. 开发功能
# 编辑代码...

# 3. 提交（带角色标签）
git add .
git commit -m "[角色] 类型：描述"

# 4. 推送到远程
git push origin dev
```

---

## ⚠️ 注意事项

### 1️⃣ SSH Key 管理
- 统一使用 `redfalsh1` 账号的 SSH Key
- 确保 SSH Key 已添加到 GitHub
- 测试连接：`ssh -T git@github.com`

### 2️⃣ 权限管理
- 所有成员使用同一账号推送
- 通过角色标签区分贡献者
- TechLead 负责代码审查

### 3️⃣ 冲突处理
- 推送前务必 `git pull`
- 解决冲突后标注 `[TL] merge:`
- 保持提交历史清晰

---

## 📈 贡献统计

### 按角色统计
```bash
# 统计各角色提交数量
git log --oneline | grep -c "\[TL\]"
git log --oneline | grep -c "\[CPP\]"
git log --oneline | grep -c "\[QT\]"
git log --oneline | grep -c "\[QA\]"
git log --oneline | grep -c "\[OPS\]"
```

### 按类型统计
```bash
# 统计提交类型
git log --oneline | grep -c "feat:"
git log --oneline | grep -c "fix:"
git log --oneline | grep -c "docs:"
```

---

## 🔗 相关文档

- [需求文档](../requirements/README.md)
- [开发环境配置](SETUP.md)
- [代码规范](CODE_STYLE.md)

---

## 📝 更新日志

| 日期 | 版本 | 更新内容 | 作者 |
|------|------|----------|------|
| 2026-03-30 | v1.0 | 初始版本，定义角色标签规范 | eda_tech_lead |

---

**所有成员请遵守此规范！** 🚀
