# REQ-401 - GDSII 支持

**版本**: v1.0  
**优先级**: 🔴 高  
**状态**: 🔄 开发中  
**最后更新**: 2026-03-30

---

## 📋 描述

支持 GDSII 格式的导入和导出，这是 EDA 行业的标准数据格式。

---

## 📐 功能要求

### GDSII 导入
- 读取 .gds 二进制文件
- 解析结构 (Structure)
- 解析图层 (Layer)
- 解析图形类型:
  - Boundary (多边形)
  - Path (路径)
  - Text (文本)
  - Box (矩形)
  - Array (阵列)
- 支持层次化结构
- 支持实例引用

### GDSII 导出
- 导出为 .gds 文件
- 保持层次结构
- 保持图层信息
- 支持选择导出范围

### 导入选项
- 图层过滤
- 结构选择
- 单位转换
- 坐标缩放

---

## 🔧 技术要求

- 二进制流式解析
- 大文件支持 (>1GB)
- 内存高效 (流式处理)
- 错误处理完善
- 进度显示

---

## 📁 GDSII 文件结构

```
GDSII 文件
├── HEADER (版本)
├── BGNLIB (库开始)
│   ├── LIBNAME (库名)
│   ├── UNITS (单位)
│   ├── BGNSTR (结构开始)
│   │   ├── STRNAME (结构名)
│   │   ├── BOUNDARY (边界)
│   │   │   ├── LAYER
│   │   │   ├── DATATYPE
│   │   │   └── XY (坐标)
│   │   ├── PATH (路径)
│   │   ├── TEXT (文本)
│   │   ├── SREF (结构引用)
│   │   ├── AREF (阵列引用)
│   │   └── ENDSTR (结构结束)
│   └── ENDLIB (库结束)
```

---

## 📁 相关文件

| 文件 | 说明 |
|------|------|
| `src/io/gdsii/gds_parser.cpp` | GDSII 解析器 |
| `include/io/gdsii/gds_parser.h` | GDSII 解析器头文件 |
| `src/io/gdsii/gds_writer.cpp` | GDSII 写入器 |
| `src/io/gdsii/gds_types.cpp` | GDSII 类型定义 |

---

## ✅ 验收标准

- [ ] 能解析标准 GDSII 文件
- [ ] 支持所有图形类型
- [ ] 支持层次化结构
- [ ] 导出文件可被其他工具读取
- [ ] 大文件 (>100MB) 正常处理
- [ ] 错误文件正确处理

---

## 🔗 相关文档

- [OASIS 支持](002-oasis-support.md)
- [版图数据结构](../01-layout/001-layout-editor.md)

---

## 📝 更新日志

| 日期 | 版本 | 更新内容 | 作者 |
|------|------|----------|------|
| 2026-03-30 | v1.0 | 初始版本 | eda_tech_lead |
