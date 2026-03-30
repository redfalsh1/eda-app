#ifndef PLUGIN_PLUGIN_INTERFACE_H
#define PLUGIN_PLUGIN_INTERFACE_H

#include <QString>
#include <memory>
#include <vector>

// ============================================================================
// 插件接口定义
// ============================================================================

/**
 * @brief 插件接口 (纯虚类)
 * 
 * 所有插件必须实现此接口
 * 插件应以动态库 (.dll/.so/.dylib) 形式提供
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    /**
     * @brief 插件名称
     */
    virtual QString name() const = 0;
    
    /**
     * @brief 插件版本
     */
    virtual QString version() const = 0;
    
    /**
     * @brief 插件描述
     */
    virtual QString description() const { return QString(); }
    
    /**
     * @brief 初始化插件
     * 
     * 在插件加载后调用，进行初始化工作
     * @return true 表示初始化成功
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief 关闭插件
     * 
     * 在插件卸载前调用，进行清理工作
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief 获取依赖的插件列表
     */
    virtual QStringList dependencies() const { return QStringList(); }
    
    /**
     * @brief 是否支持自动加载
     */
    virtual bool autoLoad() const { return false; }
};

// ============================================================================
// 插件工厂接口
// ============================================================================

/**
 * @brief 插件工厂接口
 * 
 * 用于创建插件实例
 */
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    
    /**
     * @brief 创建插件实例
     */
    virtual std::unique_ptr<IPlugin> createPlugin() = 0;
};

// ============================================================================
// 插件元数据
// ============================================================================

namespace plugin {

/**
 * @brief 插件信息
 */
struct PluginInfo {
    QString name;
    QString version;
    QString description;
    QString filePath;
    bool loaded{false};
    bool autoLoad{false};
    QStringList dependencies;
    
    bool isValid() const {
        return !name.isEmpty() && !filePath.isEmpty();
    }
};

/**
 * @brief 插件状态
 */
enum class PluginStatus {
    Unloaded,      ///< 未加载
    Loaded,        ///< 已加载
    Initialized,   ///< 已初始化
    Error          ///< 错误
};

} // namespace plugin

// ============================================================================
// 插件导出宏 (用于 Windows DLL)
// ============================================================================

#ifdef _WIN32
    #ifdef PLUGIN_EXPORTS
        #define PLUGIN_API __declspec(dllexport)
    #else
        #define PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define PLUGIN_API
#endif

// ============================================================================
// 插件导出函数 (C 接口)
// ============================================================================

extern "C" {
    /**
     * @brief 获取插件工厂
     * 
     * 每个插件 DLL 必须导出此函数
     */
    typedef IPluginFactory* (*GetPluginFactoryFunc)();
    
    /**
     * @brief 获取插件 API 版本
     * 
     * 用于检查插件兼容性
     */
    typedef int (*GetPluginApiVersionFunc)();
}

/**
 * @brief 当前插件 API 版本
 * 
 * 插件应返回此版本号以确保兼容性
 */
constexpr int PLUGIN_API_VERSION = 1;

#endif // PLUGIN_PLUGIN_INTERFACE_H
