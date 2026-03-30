#include "plugin/plugin_manager.h"
#include "plugin/plugin_loader.h"
#include <QDir>
#include <QFileInfo>
#include <algorithm>
#include <set>

namespace plugin {

// ============================================================================
// PluginManager 实现
// ============================================================================

PluginManager& PluginManager::instance() {
    static PluginManager instance;
    return instance;
}

PluginManager::PluginManager() {
    // 添加默认插件目录
    addPluginPath("./plugins");
}

PluginManager::~PluginManager() {
    unloadAllPlugins();
}

// -----------------------------------------------------------------------------
// 插件目录管理
// -----------------------------------------------------------------------------

void PluginManager::addPluginPath(const QString& path) {
    if (!pluginPaths_.contains(path)) {
        pluginPaths_.append(path);
    }
}

QStringList PluginManager::pluginPaths() const {
    return pluginPaths_;
}

// -----------------------------------------------------------------------------
// 插件发现
// -----------------------------------------------------------------------------

void PluginManager::scanPlugins() {
    availablePlugins_.clear();
    
    for (const QString& path : pluginPaths_) {
        QDir dir(path);
        if (!dir.exists()) {
            continue;
        }
        
        // 查找动态库文件
        QStringList filters;
#ifdef _WIN32
        filters << "*.dll";
#elif defined(__APPLE__)
        filters << "*.dylib";
#else
        filters << "*.so" << "*.so.*";
#endif
        
        dir.setNameFilters(filters);
        QFileInfoList files = dir.entryInfoList(QDir::Files);
        
        for (const QFileInfo& file : files) {
            QString name = extractPluginName(file.fileName());
            
            PluginInfo info;
            info.name = name;
            info.filePath = file.absoluteFilePath();
            info.loaded = false;
            info.autoLoad = false;
            
            availablePlugins_[name] = info;
        }
    }
}

PluginManager::PluginList PluginManager::availablePlugins() const {
    PluginList list;
    for (const auto& [name, info] : availablePlugins_) {
        list.push_back(info);
    }
    return list;
}

PluginManager::PluginList PluginManager::loadedPlugins() const {
    PluginList list;
    for (const auto& [name, info] : availablePlugins_) {
        if (info.loaded) {
            list.push_back(info);
        }
    }
    return list;
}

bool PluginManager::isPluginAvailable(const QString& name) const {
    return availablePlugins_.count(name.toLower().toStdString()) > 0;
}

// -----------------------------------------------------------------------------
// 插件加载/卸载
// -----------------------------------------------------------------------------

bool PluginManager::loadPlugin(const QString& name) {
    QString key = name.toLower();
    
    if (availablePlugins_.find(key.toStdString()) == availablePlugins_.end()) {
        lastError_ = QString("Plugin not found: %1").arg(name);
        return false;
    }
    
    if (isPluginLoaded(name)) {
        return true;  // 已经加载
    }
    
    // 解析依赖
    QStringList loadOrder;
    if (!resolveDependencies(name, loadOrder)) {
        lastError_ = QString("Failed to resolve dependencies for: %1").arg(name);
        return false;
    }
    
    // 按依赖顺序加载
    for (const QString& pluginName : loadOrder) {
        if (isPluginLoaded(pluginName)) {
            continue;
        }
        
        auto it = availablePlugins_.find(pluginName.toLower().toStdString());
        if (it == availablePlugins_.end()) {
            lastError_ = QString("Dependency not found: %1").arg(pluginName);
            return false;
        }
        
        PluginInfo& info = it->second;
        
        // 创建加载器
        auto loader = std::make_unique<PluginLoader>(info.filePath);
        
        // 加载动态库
        if (!loader->load()) {
            lastError_ = QString("Failed to load plugin %1: %2").arg(pluginName, loader->lastError());
            setPluginStatus(pluginName, PluginStatus::Error, lastError_);
            return false;
        }
        
        // 创建插件实例
        auto plugin = loader->createPlugin();
        if (!plugin) {
            lastError_ = QString("Failed to create plugin instance: %1").arg(pluginName);
            setPluginStatus(pluginName, PluginStatus::Error, lastError_);
            return false;
        }
        
        // 初始化插件
        if (!plugin->initialize()) {
            lastError_ = QString("Failed to initialize plugin: %1").arg(pluginName);
            setPluginStatus(pluginName, PluginStatus::Error, lastError_);
            return false;
        }
        
        // 保存插件和加载器
        info.loaded = true;
        info.autoLoad = plugin->autoLoad();
        info.dependencies = plugin->dependencies();
        loadedPlugins_[pluginName.toLower().toStdString()] = std::move(plugin);
        loaders_[pluginName.toLower().toStdString()] = std::move(loader);
        
        setPluginStatus(pluginName, PluginStatus::Initialized);
    }
    
    return true;
}

bool PluginManager::unloadPlugin(const QString& name) {
    QString key = name.toLower();
    
    auto it = loadedPlugins_.find(key.toStdString());
    if (it == loadedPlugins_.end()) {
        return true;  // 未加载，视为成功
    }
    
    // 检查是否有其他插件依赖此插件
    for (const auto& [pluginName, plugin] : loadedPlugins_) {
        if (pluginName == key.toStdString()) {
            continue;
        }
        
        if (plugin->dependencies().contains(name)) {
            lastError_ = QString("Cannot unload %1: plugin %2 depends on it")
                .arg(name, QString::fromStdString(pluginName));
            return false;
        }
    }
    
    // 关闭插件
    it->second->shutdown();
    
    // 移除加载器和插件
    loaders_.erase(key.toStdString());
    loadedPlugins_.erase(key.toStdString());
    
    // 更新状态
    auto infoIt = availablePlugins_.find(key.toStdString());
    if (infoIt != availablePlugins_.end()) {
        infoIt->second.loaded = false;
    }
    
    setPluginStatus(name, PluginStatus::Unloaded);
    
    return true;
}

bool PluginManager::isPluginLoaded(const QString& name) const {
    return loadedPlugins_.count(name.toLower().toStdString()) > 0;
}

IPlugin* PluginManager::getPlugin(const QString& name) {
    auto it = loadedPlugins_.find(name.toLower().toStdString());
    return (it != loadedPlugins_.end()) ? it->second.get() : nullptr;
}

const IPlugin* PluginManager::getPlugin(const QString& name) const {
    auto it = loadedPlugins_.find(name.toLower().toStdString());
    return (it != loadedPlugins_.end()) ? it->second.get() : nullptr;
}

// -----------------------------------------------------------------------------
// 批量操作
// -----------------------------------------------------------------------------

void PluginManager::loadAutoLoadPlugins() {
    scanPlugins();
    
    for (const auto& [name, info] : availablePlugins_) {
        // 尝试加载以获取 autoLoad 信息
        auto loader = std::make_unique<PluginLoader>(info.filePath);
        if (loader->load()) {
            auto plugin = loader->createPlugin();
            if (plugin && plugin->autoLoad()) {
                loadPlugin(QString::fromStdString(name));
            }
        }
    }
}

void PluginManager::loadAllPlugins() {
    scanPlugins();
    
    for (const auto& [name, info] : availablePlugins_) {
        loadPlugin(QString::fromStdString(name));
    }
}

void PluginManager::unloadAllPlugins() {
    // 反向卸载 (依赖的插件先卸载)
    std::vector<QString> names;
    for (const auto& [name, plugin] : loadedPlugins_) {
        names.push_back(QString::fromStdString(name));
    }
    
    std::reverse(names.begin(), names.end());
    
    for (const QString& name : names) {
        unloadPlugin(name);
    }
}

// -----------------------------------------------------------------------------
// 状态回调
// -----------------------------------------------------------------------------

void PluginManager::setStatusCallback(StatusCallback callback) {
    statusCallback_ = std::move(callback);
}

// -----------------------------------------------------------------------------
// 工具函数
// -----------------------------------------------------------------------------

PluginManager::PluginInfo PluginManager::getPluginInfo(const QString& name) const {
    auto it = availablePlugins_.find(name.toLower().toStdString());
    if (it != availablePlugins_.end()) {
        return it->second;
    }
    return PluginInfo();
}

PluginStatus PluginManager::getPluginStatus(const QString& name) const {
    auto it = pluginStatuses_.find(name.toLower().toStdString());
    if (it != pluginStatuses_.end()) {
        return it->second;
    }
    return PluginStatus::Unloaded;
}

// -----------------------------------------------------------------------------
// 私有方法
// -----------------------------------------------------------------------------

bool PluginManager::resolveDependencies(const QString& name, QStringList& order) {
    std::set<QString> visited;
    std::vector<QString> result;
    
    std::function<bool(const QString&)> visit = [&](const QString& n) -> bool {
        if (visited.count(n)) {
            return true;  // 已访问
        }
        
        visited.insert(n);
        
        // 获取插件信息
        auto it = availablePlugins_.find(n.toLower().toStdString());
        if (it == availablePlugins_.end()) {
            return false;  // 插件不存在
        }
        
        // 临时加载以获取依赖
        PluginLoader loader(it->second.filePath);
        if (loader.load()) {
            auto plugin = loader.createPlugin();
            if (plugin) {
                for (const QString& dep : plugin->dependencies()) {
                    if (!visit(dep)) {
                        return false;
                    }
                }
            }
        }
        
        result.push_back(n);
        return true;
    };
    
    if (!visit(name)) {
        return false;
    }
    
    order = QStringList(result.begin(), result.end());
    return true;
}

bool PluginManager::checkDependencies(const IPlugin* plugin) const {
    if (!plugin) return false;
    
    for (const QString& dep : plugin->dependencies()) {
        if (!isPluginLoaded(dep)) {
            return false;
        }
    }
    return true;
}

void PluginManager::setPluginStatus(const QString& name, PluginStatus status, const QString& message) {
    pluginStatuses_[name.toLower().toStdString()] = status;
    
    if (statusCallback_) {
        statusCallback_(name, status, message);
    }
}

QString PluginManager::extractPluginName(const QString& filePath) {
    QFileInfo fi(filePath);
    QString baseName = fi.baseName();
    
    // 移除平台前缀/后缀
    if (baseName.startsWith("lib")) {
        baseName = baseName.mid(3);
    }
    
    return baseName.toLower();
}

} // namespace plugin
