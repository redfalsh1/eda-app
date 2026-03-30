# REQ-301 - 插件框架

**版本**: v1.0  
**优先级**: 🔴 高  
**状态**: 🔄 开发中  
**最后更新**: 2026-03-30

---

## 📋 描述

EDA 应用的插件系统框架，支持动态加载/卸载插件。

---

## 🏗️ 架构设计

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层                                  │
│  ┌─────────────────┐        ┌─────────────────┐             │
│  │  版图编辑器      │        │  插件管理器      │             │
│  │  (主程序)       │◄──────►│  PluginManager  │             │
│  └─────────────────┘        └─────────────────┘             │
│                                │                             │
│                                ▼                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                  插件接口层                           │    │
│  │  IPlugin (纯虚接口)                                  │    │
│  └─────────────────────────────────────────────────────┘    │
│                                │                             │
│           ┌────────────────────┼────────────────────┐       │
│           ▼                    ▼                    ▼       │
│  ┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐│
│  │  原理图插件      │ │  网表生成器     │ │  3D 预览器      ││
│  │  (动态库)       │ │  (动态库)       │ │  (动态库)       ││
│  └─────────────────┘ └─────────────────┘ └─────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

---

## 📐 功能要求

### 插件接口
```cpp
// include/plugin/iplugin.h
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // 基本信息
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;
    
    // 生命周期
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    
    // UI 集成 (可选)
    virtual QWidget* mainWidget() { return nullptr; }
    virtual QMenu* createMenu() { return nullptr; }
    virtual QToolBar* createToolBar() { return nullptr; }
    
    // 事件处理 (可选)
    virtual bool handleEvent(const Event& event) { return false; }
};
```

### 插件管理器
- 扫描插件目录
- 加载/卸载插件
- 插件注册
- 插件依赖管理
- 插件冲突检测

### 插件生命周期
1. **发现**: 扫描 `plugins/` 目录
2. **加载**: 动态库加载
3. **初始化**: 调用 `initialize()`
4. **运行**: 正常提供服务
5. **关闭**: 调用 `shutdown()`
6. **卸载**: 释放动态库

---

## 🔧 技术要求

- 使用 Qt Plugin 系统
- 插件为独立动态库 (`.dll`/`.so`)
- 接口版本控制
- 支持插件热加载 (可选)

---

## 📁 相关文件

| 文件 | 说明 |
|------|------|
| `include/plugin/iplugin.h` | 插件接口定义 |
| `src/plugin/plugin_manager.cpp` | 插件管理器 |
| `src/plugin/plugin_loader.cpp` | 插件加载器 |
| `plugins/schematic/main.cpp` | 原理图插件示例 |

---

## 📁 插件目录结构

```
plugins/
├── schematic/              # 原理图插件
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── schematic_editor.cpp
│   └── schematic_editor.h
├── netlist/                # 网表生成器
│   └── ...
└── viewer3d/               # 3D 预览器
    └── ...
```

---

## ✅ 验收标准

- [ ] 插件接口定义完整
- [ ] 插件可动态加载/卸载
- [ ] 插件管理器功能正常
- [ ] 支持多个插件同时运行
- [ ] 插件崩溃不影响主程序

---

## 🔗 相关文档

- [原理图插件](002-schematic-plugin.md)
- [插件 API](003-plugin-api.md)
- [架构需求](../00-master/002-architecture.md)

---

## 📝 更新日志

| 日期 | 版本 | 更新内容 | 作者 |
|------|------|----------|------|
| 2026-03-30 | v1.0 | 初始版本 | eda_tech_lead |
