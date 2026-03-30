# EDA 开发环境配置指南

本文档介绍如何配置 EDA 项目的开发环境。

---

## 📋 系统要求

### Windows

| 组件 | 版本 | 下载地址 |
|------|------|----------|
| 操作系统 | Windows 10+ | - |
| 编译器 | MSVC 2019/2022 | [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) |
| Qt | Qt 5.15.2 | [Qt Online Installer](https://www.qt.io/download) |
| CMake | 3.16+ | [CMake Download](https://cmake.org/download/) |
| Git | 最新版 | [Git for Windows](https://git-scm.com/download/win) |

### Linux (Ubuntu 20.04+)

| 组件 | 版本 | 安装命令 |
|------|------|----------|
| 编译器 | GCC 9+ | `sudo apt install build-essential` |
| Qt | Qt 5.15.2 | `sudo apt install qt5-default` |
| CMake | 3.16+ | `sudo apt install cmake` |
| Git | 最新版 | `sudo apt install git` |

---

## 🔧 安装步骤

### Windows 安装指南

#### 1. 安装 Visual Studio Build Tools

```bash
# 下载并运行 Visual Studio Build Tools 安装器
# 选择以下工作负载：
# - Desktop development with C++
# - Windows 10 SDK
```

**验证安装**:
```bash
cl
# 应显示 MSVC 版本信息
```

---

#### 2. 安装 Qt 5.15.2

```bash
# 下载 Qt Online Installer
# 运行安装器，选择：
# - Qt 5.15.2
# - MSVC 2019 64-bit 或 MSVC 2022 64-bit
# - Qt Charts (可选)
# - Qt WebSockets (可选)
```

**配置环境变量**:
```bash
# 添加 Qt 到 PATH
C:\Qt\5.15.2\msvc2019_64\bin
```

**验证安装**:
```bash
qmake --version
# 应显示 Qt 5.15.2
```

---

#### 3. 安装 CMake

```bash
# 下载 CMake 安装器
# 安装时选择 "Add CMake to the system PATH"
```

**验证安装**:
```bash
cmake --version
# 应显示 CMake 3.16+
```

---

#### 4. 安装 Git

```bash
# 下载 Git for Windows
# 运行安装器，使用默认选项
```

**配置 Git**:
```bash
git config --global user.name "Your Name"
git config --global user.email "your@email.com"
```

**验证安装**:
```bash
git --version
```

---

### Linux 安装指南

#### 1. 安装基础工具

```bash
sudo apt update
sudo apt install -y build-essential git cmake
```

#### 2. 安装 Qt 5

```bash
sudo apt install -y qt5-default qtbase5-dev qtdeclarative5-dev
```

**验证安装**:
```bash
qmake --version
cmake --version
git --version
```

---

## 📦 克隆项目

```bash
# 克隆仓库
git clone https://github.com/redfalsh1/eda-app.git
cd eda-app

# 查看分支
git branch -a
# main
# dev

# 创建功能分支
git checkout -b feature/my-feature dev
```

---

## 🔨 构建项目

### Windows

```bash
# 配置
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build --config Release -j8

# 测试
cd build
ctest --output-on-failure

# 运行
.\build\bin\Release\EDA.exe
```

### Linux

```bash
# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build -j8

# 测试
cd build
ctest --output-on-failure

# 运行
./build/bin/EDA
```

---

## 🧪 运行测试

```bash
# 运行所有测试
ctest --output-on-failure

# 运行特定测试
ctest -R TestName --verbose

# 带覆盖率运行 (Linux)
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
cmake --build build
ctest -T Coverage
```

---

## 📝 代码规范

### C++ 编码规范

- 遵循 C++17 标准
- 使用 Qt 编码规范
- 命名约定:
  - 类名：`PascalCase`
  - 函数名：`camelCase`
  - 成员变量：`m_variableName`
  - 常量：`UPPER_CASE`

### 提交规范

```bash
# 提交消息格式
<type>(<scope>): <subject>

# 类型:
# feat: 新功能
# fix: Bug 修复
# docs: 文档更新
# style: 代码格式
# refactor: 重构
# test: 测试
# chore: 构建/工具

# 示例:
git commit -m "feat(ui): add main window layout"
git commit -m "fix(build): resolve CMake configuration issue"
```

### 代码格式化

```bash
# 使用 clang-format (可选)
clang-format -i src/*.cpp include/*.h
```

---

## 🔍 常见问题

### 问题 1: CMake 找不到 Qt

**错误**: `Could not find Qt5`

**解决**:
```bash
# Windows: 指定 CMAKE_PREFIX_PATH
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64"

# Linux:
cmake -B build -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake/Qt5
```

---

### 问题 2: 构建失败 - 找不到 MSVC

**错误**: `No CMAKE_CXX_COMPILER could be found`

**解决**:
```bash
# 确保安装了 Visual Studio Build Tools
# 使用正确的 Generator
cmake -B build -G "Visual Studio 17 2022"
```

---

### 问题 3: Qt 模块缺失

**错误**: `By not providing "FindQt5Charts.cmake"`

**解决**:
```bash
# 安装缺失的 Qt 模块
# Windows: 运行 Qt Maintenance Tool 添加模块
# Linux:
sudo apt install qtmultimedia5-dev qtwebsockets5-dev
```

---

### 问题 4: Git 推送失败

**错误**: `Permission denied (publickey)`

**解决**:
```bash
# 使用 HTTPS 而非 SSH
git remote set-url origin https://github.com/redfalsh1/eda-app.git

# 或配置 SSH 密钥
ssh-keygen -t ed25519 -C "your@email.com"
# 将公钥添加到 GitHub: Settings → SSH and GPG keys
```

---

### 问题 5: CI 构建失败

**排查步骤**:
1. 查看 GitHub Actions 日志
2. 本地复现相同配置
3. 检查 CMakeLists.txt 语法
4. 验证依赖版本

**常见原因**:
- 依赖版本不匹配
- 路径问题（大小写敏感）
- 缺少必要的 Qt 模块

---

## 🛠️ IDE 配置

### Visual Studio Code

安装扩展:
- C/C++ (Microsoft)
- CMake Tools (Microsoft)
- Qt Tools (benjamin-simmonds)

**settings.json**:
```json
{
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "C_Cpp.default.compilerPath": "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/*/bin/Hostx64/x64/cl.exe"
}
```

### Qt Creator

1. 打开 `CMakeLists.txt`
2. 自动配置 CMake
3. 选择构建配置（Debug/Release）
4. 开始开发！

### Visual Studio

```bash
# 生成 Visual Studio 解决方案
cmake -B build -G "Visual Studio 17 2022"

# 打开 build/EDA.sln
```

---

## 📚 参考资源

- [Qt 文档](https://doc.qt.io/)
- [CMake 文档](https://cmake.org/cmake/help/latest/)
- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [C++17 特性](https://en.cppreference.com/w/cpp/17)

---

## 📞 获取帮助

遇到问题？

1. 查看本文档的常见问题部分
2. 查看项目 `README.md`
3. 在 GitHub 提交 Issue
4. 联系团队成员

**团队联系方式**:
- TechLead: eda_tech_lead
- DevOps: eda_devops

---

_最后更新_: 2026-03-30  
_版本_: 1.0.0
