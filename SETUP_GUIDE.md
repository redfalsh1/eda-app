# GitHub 仓库配置 - 快速开始指南

## 📋 任务清单

### 前置条件
- [ ] 已安装 GitHub CLI (`gh --version`)
- [ ] 已完成 GitHub 认证 (`gh auth login`)
- [ ] 已安装 Git

### 步骤 1: GitHub 认证 ⚠️ **必须首先完成**

```bash
# 运行认证命令
gh auth login
```

**认证流程：**
1. 选择 `GitHub.com`
2. 选择 `HTTPS`
3. 选择 `Login with a web browser`
4. 复制设备码，打开 https://github.com/login/device
5. 在浏览器中授权
6. 返回终端完成认证

**验证认证：**
```bash
gh auth status
```

应显示：
```
✓ Logged in to github.com as <your-username>
```

---

### 步骤 2: 初始化 Git 仓库

```bash
cd eda-app
git init
git add .
git commit -m "Initial commit: EDA project structure"
git branch -M main
```

---

### 步骤 3: 创建 GitHub 仓库

**选项 A: 使用脚本（推荐）**
```bash
.\scripts\setup-github.ps1
```

**选项 B: 手动命令**
```bash
# 创建仓库
gh repo create eda-app --public --description "EDA Desktop Application"

# 添加远程
git remote add origin https://github.com/owner/eda-app.git

# 推送
git push -u origin main
```

---

### 步骤 4: 创建 dev 分支

```bash
git checkout -b dev
git push -u origin dev
git checkout main
```

---

### 步骤 5: 配置分支保护规则 ⚠️ **需要 GitHub UI**

**访问：** https://github.com/owner/eda-app/settings/branches

#### main 分支保护规则：
- ✅ **Require a pull request before merging**
  - Required approvers: `1`
  - ✅ Dismiss stale pull request approvals when new commits are pushed
- ✅ **Require status checks to pass before merging**
  - Required status checks:
    - `CI (windows-latest, 5.15.2, Debug)`
    - `CI (windows-latest, 5.15.2, Release)`
    - `CI (ubuntu-20.04, 5.15.2, Debug)`
    - `CI (ubuntu-20.04, 5.15.2, Release)`
  - ✅ Require branches to be up to date before merging
- ✅ **Do not allow bypassing the above requirements**
- ✅ **Require conversation resolution before merging**

#### dev 分支：
- ❌ 无保护规则（允许直接推送）

---

### 步骤 6: 添加团队成员

**访问：** https://github.com/owner/eda-app/settings/access

**添加以下成员（使用实际 GitHub 用户名）：**

| 角色 | GitHub 用户名 | 权限级别 |
|------|--------------|----------|
| TechLead | `<tech_lead_username>` | Admin |
| C++ Dev | `<cpp_dev_username>` | Write |
| Qt Dev | `<qt_dev_username>` | Write |
| DevOps | `<devops_username>` | Write |
| QA | `<qa_username>` | Write |

**或使用脚本：**
```bash
.\scripts\setup-team.ps1
# 编辑脚本中的 TEAM_MEMBERS 数组，填入实际用户名
```

---

### 步骤 7: 启用 GitHub Actions

**访问：** https://github.com/owner/eda-app/actions

- 点击 "I understand my workflows, go ahead and enable them"
- 确认 CI 和 Release 工作流已启用

---

## ✅ 验收标准

完成所有步骤后，验证以下项目：

### 仓库结构
- [ ] README.md 存在且内容正确
- [ ] LICENSE 存在（MIT）
- [ ] .gitignore 存在（C++/Qt/CMake）
- [ ] docs/ 目录存在
- [ ] .github/workflows/ 包含 ci.yml, release.yml, codeql.yml

### 分支配置
- [ ] main 分支存在且受保护
- [ ] dev 分支存在且可推送
- [ ] 分支保护规则已配置

### 团队权限
- [ ] 所有 5 名成员已添加
- [ ] 权限级别正确（TechLead=Admin, 其他=Write）

### CI/CD
- [ ] GitHub Actions 已启用
- [ ] 推送后 CI 自动运行
- [ ] 状态检查显示在 PR 中

---

## 🚀 验证测试

### 测试 1: 推送测试
```bash
# 创建测试分支
git checkout -b test-branch
echo "# Test" >> README.md
git add README.md
git commit -m "test: verify push access"
git push -u origin test-branch

# 创建 PR 测试 CI
gh pr create --title "Test PR" --body "Testing CI/CD" --base main
```

### 测试 2: 验证 CI
- 访问 https://github.com/owner/eda-app/actions
- 确认 CI 工作流正在运行或已完成
- 所有检查应为绿色 ✓

### 测试 3: 验证分支保护
- 尝试直接推送到 main：
  ```bash
  git checkout main
  echo "test" >> test.txt
  git add test.txt
  git commit -m "test"
  git push origin main  # 应该被拒绝！
  ```
- 应收到错误：`protected branch hook declined`

---

## 📞 故障排查

### 问题：gh auth login 失败
**解决：**
```bash
# 清除旧认证
gh auth logout

# 重新认证
gh auth login
```

### 问题：仓库已存在
**解决：**
```bash
# 如果仓库已存在，直接推送
cd eda-app
git remote add origin https://github.com/owner/eda-app.git
git push -u origin main
```

### 问题：CI 不运行
**解决：**
1. 检查 Actions 是否启用
2. 检查 .github/workflows/ 文件语法
3. 查看 https://github.com/owner/eda-app/settings/actions

### 问题：无法配置分支保护
**解决：**
- 确认你是仓库管理员
- 使用 GitHub UI 而非 API（更直观）
- 访问：Settings → Branches → Add branch protection rule

---

## 📚 相关文档

- [GitHub CLI 文档](https://cli.github.com/manual/)
- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [分支保护规则](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-protected-branches/about-protected-branches)

---

**配置完成后，将仓库 URL 同步给团队！**

🔗 仓库 URL: `https://github.com/owner/eda-app`

---

_EDA DevOps Team - 2026_
