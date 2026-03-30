# GitHub 仓库配置 - 验收清单

## 任务信息

- **任务 ID**: GH-SETUP-001
- **优先级**: 🔴 高
- **负责人**: eda_devops (运维)
- **截止时间**: 2026-03-30 17:00
- **状态**: 🟡 进行中（等待认证）

---

## ✅ 验收标准

### 1. 仓库创建完成

| 检查项 | 状态 | 验证方法 |
|--------|------|----------|
| 仓库 URL 可访问 | ⏳ 待验证 | https://github.com/owner/eda-app |
| 仓库为 Public | ⏳ 待验证 | 查看仓库图标（公开） |
| 描述正确 | ⏳ 待验证 | "EDA Desktop Application" |

**完成标志**：
- [ ] 仓库创建成功
- [ ] 可正常访问

---

### 2. 分支保护配置正确

| 检查项 | 状态 | 验证方法 |
|--------|------|----------|
| main 分支需要 PR 审查 | ⏳ 待验证 | Settings → Branches |
| main 分支需要 CI 通过 | ⏳ 待验证 | 查看 branch protection rules |
| dev 分支允许直接推送 | ⏳ 待验证 | 尝试 git push origin dev |

**main 分支保护规则**：
- [ ] Require a pull request before merging (1 approver)
- [ ] Require status checks to pass before merging
- [ ] Require branches to be up to date before merging
- [ ] Do not allow bypassing the above requirements

**完成标志**：
- [ ] main 分支保护规则已配置
- [ ] dev 分支无保护规则

---

### 3. 所有成员有写入权限

| 成员 | 角色 | 权限 | 状态 |
|------|------|------|------|
| eda_tech_lead | TechLead | Admin | ⏳ 待添加 |
| eda_cpp_dev | Developer | Write | ⏳ 待添加 |
| eda_qt_dev | Developer | Write | ⏳ 待添加 |
| eda_devops | DevOps | Write | ⏳ 待添加 |
| eda_qa | QA | Write | ⏳ 待添加 |

**验证方法**：
- 访问 https://github.com/owner/eda-app/settings/access
- 确认所有成员在列表中
- 确认权限级别正确

**完成标志**：
- [ ] 5 名成员全部添加
- [ ] 权限配置正确

---

### 4. 基础文件结构完整

| 文件/目录 | 状态 | 验证方法 |
|-----------|------|----------|
| README.md | ✅ 已创建 | `git ls-files README.md` |
| LICENSE | ✅ 已创建 | `git ls-files LICENSE` |
| .gitignore | ✅ 已创建 | `git ls-files .gitignore` |
| docs/ | ✅ 已创建 | `git ls-files docs/` |
| .github/workflows/ | ✅ 已创建 | `git ls-files .github/workflows/` |
| src/ | ✅ 已创建 | `git ls-files src/` |
| include/ | ✅ 已创建 | `git ls-files include/` |
| tests/ | ✅ 已创建 | `git ls-files tests/` |
| CMakeLists.txt | ✅ 已创建 | `git ls-files CMakeLists.txt` |

**CI/CD 配置文件**：
- [ ] ci.yml - CI 流水线
- [ ] release.yml - 发布流水线
- [ ] codeql.yml - 代码扫描

**完成标志**：
- [ ] 所有文件已推送到仓库
- [ ] 文件内容正确

---

## 📊 完成进度

| 阶段 | 进度 | 状态 |
|------|------|------|
| 文件准备 | 100% | ✅ 完成 |
| GitHub 认证 | 0% | ⏳ 等待用户 |
| 仓库创建 | 0% | ⏸️ 阻塞 |
| 分支保护 | 0% | ⏸️ 阻塞 |
| 团队权限 | 0% | ⏸️ 阻塞 |
| 文件推送 | 0% | ⏸️ 阻塞 |
| CI/CD验证 | 0% | ⏸️ 阻塞 |

**总体进度**: 20%（文件准备完成，等待认证）

---

## 🚧 阻塞问题

### 阻塞 1: GitHub CLI 认证

**影响**：无法创建仓库、配置权限、推送代码

**解决方案**：
```bash
gh auth login
```

**负责人**：需要用户执行

**状态**：⏳ 等待中

---

## 📝 执行日志

### 2026-03-30 09:11
- ✅ 创建项目目录结构
- ✅ 创建 README.md
- ✅ 创建 .gitignore（C++/Qt/CMake）
- ✅ 创建 LICENSE（MIT）
- ✅ 创建 CMakeLists.txt
- ✅ 创建 version.h.in
- ✅ 创建源代码文件（main.cpp, mainwindow.cpp, mainwindow.h）
- ✅ 创建测试文件（test_main.cpp, tests/CMakeLists.txt）
- ✅ 创建 CI/CD 配置文件（ci.yml, release.yml, codeql.yml）
- ✅ 创建构建脚本（build.ps1）
- ✅ 创建文档（DEVELOPMENT.md, SETUP_GUIDE.md）

### 2026-03-30 09:15
- ✅ 创建 GitHub 配置脚本（setup-github.ps1, setup-branch-protection.ps1, setup-team.ps1）
- ✅ 创建验收清单（CHECKLIST.md）
- ⏳ 等待 GitHub 认证

---

## 🎯 下一步行动

### 立即执行（认证后）

1. **运行认证**
   ```bash
   gh auth login
   ```

2. **创建并推送仓库**
   ```bash
   cd eda-app
   .\scripts\setup-github.ps1
   ```

3. **配置分支保护**
   - 访问 GitHub UI
   - 配置 main 分支保护规则

4. **添加团队成员**
   - 访问 Settings → Access
   - 添加 5 名成员

5. **验证 CI/CD**
   - 推送测试提交
   - 确认 GitHub Actions 运行

### 预计完成时间

- 认证后 **30 分钟** 内完成所有配置

---

## 📞 联系方式

- **负责人**：eda_devops (运维)
- **汇报对象**：eda_tech_lead
- **同步渠道**：Feishu 群聊

---

**最后更新**: 2026-03-30 09:15  
**下次更新**: 认证完成后立即更新

---

_EDA DevOps Team_
