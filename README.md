# EDA Desktop Application

[![CI](https://github.com/owner/eda-app/actions/workflows/ci.yml/badge.svg)](https://github.com/owner/eda-app/actions/workflows/ci.yml)
[![Release](https://github.com/owner/eda-app/actions/workflows/release.yml/badge.svg)](https://github.com/owner/eda-app/actions/workflows/release.yml)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## 项目简介

EDA 桌面应用程序 - 基于 Qt5 的跨平台 EDA 工具

## 功能特性

- 🎨 直观的图形界面
- 🔧 强大的 EDA 功能
- 📦 跨平台支持（Windows/Linux）
- 🚀 高性能计算引擎

## 快速开始

### 环境要求

- **操作系统**: Windows 10+ / Ubuntu 20.04+
- **编译器**: MSVC 2019+ / GCC 9+
- **Qt 版本**: Qt 5.15.2
- **CMake**: 3.16+

### 构建步骤

```bash
# 克隆仓库
git clone https://github.com/owner/eda-app.git
cd eda-app

# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build -j8

# 测试
cd build && ctest --output-on-failure

# 运行
./build/bin/eda-app
```

## 项目结构

```
eda-app/
├── src/              # 源代码
├── include/          # 头文件
├── tests/            # 单元测试
├── docs/             # 文档
├── cmake/            # CMake 模块
├── packaging/        # 打包配置
├── scripts/          # 构建脚本
└── .github/          # GitHub 配置
    └── workflows/    # CI/CD 流水线
```

## 开发指南

### 分支策略

- `main` - 稳定版本，需要 PR 审查
- `dev` - 开发分支，集成测试
- `feature/*` - 功能分支

### 代码规范

- 遵循 C++17 标准
- 使用 Qt 编码规范
- 提交前运行 `clang-format`

## CI/CD

- **CI 流水线**: 每次推送自动构建和测试
- **发布流水线**: 打标签后自动创建 Release
- **支持平台**: Windows, Linux

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License - 详见 [LICENSE](LICENSE)

## 联系方式

- 项目主页：https://github.com/owner/eda-app
- 问题反馈：https://github.com/owner/eda-app/issues

---

_EDA Team © 2026_
