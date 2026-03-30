#ifndef PLUGIN_PLUGIN_MANAGER_H
#define PLUGIN_PLUGIN_MANAGER_H

#include "plugin/plugin_interface.h"
#include <QString>
#include <QStringList>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace plugin {

// 前向声明
class PluginLoader;

/**
 * @brief 插件管理器
 * 
 * 负责：
 * - 插件发现
 * - 插件加载/卸载
 * - 插件初始化/关闭
 * - 插件依赖管理
 */
class PluginManager {
public:
    using PluginList = std::vector<PluginInfo>;
    using StatusCallback = std::function<void(const QString& name, PluginStatus status, const QString& message)>;
    
    // ========================================================================
    // 单例
    // ========================================================================
    
    static PluginManager& instance();
    
    // ========================================================================
    // 构造与析构
    // ========================================================================
    
    PluginManager();
    ~PluginManager();
    
    // 禁用拷贝
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // ========================================================================
    // 插件目录管理
    // ========================================================================
    
    /**
     * @brief 添加插件搜索目录
     */
    void addPluginPath(const QString& path);
    
    /**
     * @brief 获取所有插件搜索目录
     */
    QStringList pluginPaths() const;
    
    // ========================================================================
    // 插件发现
    // ========================================================================
    
    /**
     * @brief 扫描插件目录
     * 
     * 发现所有可用的插件
     */
    void scanPlugins();
    
    /**
     * @brief 获取所有可用插件
     */
    PluginList availablePlugins() const;
    
    /**
     * @brief 获取已加载插件
     */
    PluginList loadedPlugins() const;
    
    /**
     * @brief 检查插件是否可用
     */
    bool isPluginAvailable(const QString& name) const;
    
    // ========================================================================
    // 插件加载/卸载
    // ========================================================================
    
    /**
     * @brief 加载插件
     * 
     * @param name 插件名称
     * @return true 表示加载成功
     */
    bool loadPlugin(const QString& name);
    
    /**
     * @brief 卸载插件
     * 
     * @param name 插件名称
     * @return true 表示卸载成功
     */
    bool unloadPlugin(const QString& name);
    
    /**
     * @brief 检查插件是否已加载
     */
    bool isPluginLoaded(const QString& name) const;
    
    /**
     * @brief 获取插件实例
     */
    IPlugin* getPlugin(const QString& name);
    const IPlugin* getPlugin(const QString& name) const;
    
    // ========================================================================
    // 批量操作
    // ========================================================================
    
    /**
     * @brief 加载所有自动加载的插件
     */
    void loadAutoLoadPlugins();
    
    /**
     * @brief 加载所有插件
     */
    void loadAllPlugins();
    
    /**
     * @brief 卸载所有插件
     */
    void unloadAllPlugins();
    
    // ========================================================================
    // 状态回调
    // ========================================================================
    
    /**
     * @brief 设置状态变化回调
     */
    void setStatusCallback(StatusCallback callback);
    
    // ========================================================================
    // 工具函数
    // ========================================================================
    
    /**
     * @brief 获取插件信息
     */
    PluginInfo getPluginInfo(const QString& name) const;
    
    /**
     * @brief 获取插件状态
     */
    PluginStatus getPluginStatus(const QString& name) const;
    
    /**
     * @brief 获取最后错误信息
     */
    QString lastError() const { return lastError_; }
    
    /**
     * @brief 清除错误信息
     */
    void clearError() { lastError_.clear(); }

private:
    /**
     * @brief 解析插件依赖
     */
    bool resolveDependencies(const QString& name, QStringList& order);
    
    /**
     * @brief 检查依赖是否满足
     */
    bool checkDependencies(const IPlugin* plugin) const;
    
    /**
     * @brief 设置插件状态
     */
    void setPluginStatus(const QString& name, PluginStatus status, const QString& message = QString());
    
    /**
     * @brief 从文件路径提取插件名称
     */
    static QString extractPluginName(const QString& filePath);

private:
    QStringList pluginPaths_;
    std::unordered_map<QString, PluginInfo> availablePlugins_;
    std::unordered_map<QString, std::unique_ptr<IPlugin>> loadedPlugins_;
    std::unordered_map<QString, std::unique_ptr<PluginLoader>> loaders_;
    std::unordered_map<QString, PluginStatus> pluginStatuses_;
    StatusCallback statusCallback_;
    QString lastError_;
};

} // namespace plugin

#endif // PLUGIN_PLUGIN_MANAGER_H
