# EDA 项目需求文档

**版本**: v1.0  
**最后更新**: 2026-03-30  
**维护**: eda_tech_lead

---

## 📁 需求目录结构

```
docs/requirements/
├── README.md                 # 本文件 - 需求索引
├── 00-master/                # 总体需求
│   ├── 001-project-overview.md      # 项目概述
│   └── 002-architecture.md          # 架构需求
├── 01-layout/                # 版图编辑器
│   ├── 001-layout-editor.md         # 版图编辑器核心
│   ├── 002-canvas-widget.md         # 画布控件
│   ├── 003-shape-tools.md           # 图形工具
│   └── 004-drc-engine.md            # DRC 引擎
├── 02-ui/                    # 用户界面
│   ├── 001-main-window.md           # 主窗口
│   ├── 002-dock-widgets.md          # 可停靠面板
│   ├── 003-project-tree.md          # 项目文件树
│   └── 004-layer-manager.md         # 图层管理
├── 03-plugin/                # 插件系统
│   ├── 001-plugin-framework.md      # 插件框架
│   ├── 002-schematic-plugin.md      # 原理图插件
│   └── 003-plugin-api.md            # 插件 API
├── 04-data/                  # 数据管理
│   ├── 001-gdsii-support.md         # GDSII 支持
│   ├── 002-oasis-support.md         # OASIS 支持
│   └── 003-project-format.md        # 项目格式
└── 05-non-functional/        # 非功能需求
    ├── 001-performance.md           # 性能需求
    ├── 002-compatibility.md         # 兼容性需求
    └── 003-security.md              # 安全需求
```

---

## 📋 需求索引

### 00 - 总体需求

| ID | 需求名称 | 优先级 | 状态 | 文档 |
|----|----------|--------|------|------|
| REQ-001 | 项目概述 | 🔴 高 | ✅ 已定义 | [001-project-overview.md](00-master/001-project-overview.md) |
| REQ-002 | 架构需求 | 🔴 高 | ✅ 已定义 | [002-architecture.md](00-master/002-architecture.md) |

### 01 - 版图编辑器

| ID | 需求名称 | 优先级 | 状态 | 文档 |
|----|----------|--------|------|------|
| REQ-101 | 版图编辑器核心 | 🔴 高 | 🔄 开发中 | [001-layout-editor.md](01-layout/001-layout-editor.md) |
| REQ-102 | 画布控件 | 🔴 高 | 🔄 开发中 | [002-canvas-widget.md](01-layout/002-canvas-widget.md) |
| REQ-103 | 图形工具 | 🟡 中 | ⏳ 待开发 | [003-shape-tools.md](01-layout/003-shape-tools.md) |
| REQ-104 | DRC 引擎 | 🟡 中 | ⏳ 待开发 | [004-drc-engine.md](01-layout/004-drc-engine.md) |

### 02 - 用户界面

| ID | 需求名称 | 优先级 | 状态 | 文档 |
|----|----------|--------|------|------|
| REQ-201 | 主窗口 | 🔴 高 | 🔄 开发中 | [001-main-window.md](02-ui/001-main-window.md) |
| REQ-202 | 可停靠面板 | 🔴 高 | 🔄 开发中 | [002-dock-widgets.md](02-ui/002-dock-widgets.md) |
| REQ-203 | 项目文件树 | 🔴 高 | 🔄 开发中 | [003-project-tree.md](02-ui/003-project-tree.md) |
| REQ-204 | 图层管理 | 🔴 高 | 🔄 开发中 | [004-layer-manager.md](02-ui/004-layer-manager.md) |

### 03 - 插件系统

| ID | 需求名称 | 优先级 | 状态 | 文档 |
|----|----------|--------|------|------|
| REQ-301 | 插件框架 | 🔴 高 | 🔄 开发中 | [001-plugin-framework.md](03-plugin/001-plugin-framework.md) |
| REQ-302 | 原理图插件 | 🟡 中 | ⏳ 待开发 | [002-schematic-plugin.md](03-plugin/002-schematic-plugin.md) |
| REQ-303 | 插件 API | 🔴 高 | 🔄 开发中 | [003-plugin-api.md](03-plugin/003-plugin-api.md) |

### 04 - 数据管理

| ID | 需求名称 | 优先级 | 状态 | 文档 |
|----|----------|--------|------|------|
| REQ-401 | GDSII 支持 | 🔴 高 | 🔄 开发中 | [001-gdsii-support.md](04-data/001-gdsii-support.md) |
| REQ-402 | OASIS 支持 | 🟢 低 | ⏳ 待开发 | [002-oasis-support.md](04-data/002-oasis-support.md) |
| REQ-403 | 项目格式 | 🔴 高 | 🔄 开发中 | [003-project-format.md](04-data/003-project-format.md) |

### 05 - 非功能需求

| ID | 需求名称 | 优先级 | 状态 | 文档 |
|----|----------|--------|------|------|
| REQ-501 | 性能需求 | 🔴 高 | ✅ 已定义 | [001-performance.md](05-non-functional/001-performance.md) |
| REQ-502 | 兼容性需求 | 🔴 高 | ✅ 已定义 | [002-compatibility.md](05-non-functional/002-compatibility.md) |
| REQ-503 | 安全需求 | 🟡 中 | ⏳ 待开发 | [003-security.md](05-non-functional/003-security.md) |

---

## 📊 需求状态统计

| 状态 | 数量 | 百分比 |
|------|------|--------|
| ✅ 已定义 | 5 | 18% |
| 🔄 开发中 | 12 | 43% |
| ⏳ 待开发 | 11 | 39% |
| **总计** | **28** | **100%** |

---

## 🔧 需求管理规范

### 需求编号规则
```
REQ-XXX
  ├── 0XX - 总体需求
  ├── 1XX - 版图编辑器
  ├── 2XX - 用户界面
  ├── 3XX - 插件系统
  ├── 4XX - 数据管理
  └── 5XX - 非功能需求
```

### 需求文档模板
```markdown
# REQ-XXX - 需求名称

## 描述
简要描述需求内容

## 功能要求
- 功能点 1
- 功能点 2

## 技术要求
- 技术点 1
- 技术点 2

## 验收标准
- [ ] 验收项 1
- [ ] 验收项 2

## 相关文件
- 关联代码文件
- 关联测试文件
```

---

## 📝 更新日志

| 日期 | 版本 | 更新内容 | 作者 |
|------|------|----------|------|
| 2026-03-30 | v1.0 | 初始版本，创建需求目录结构 | eda_tech_lead |

---

**维护说明**: 每次新增需求时，请更新本索引并创建对应的需求文档。
