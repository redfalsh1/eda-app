#ifndef PLUGIN_PLUGIN_LOADER_H
#define PLUGIN_PLUGIN_LOADER_H

#include "plugin/plugin_interface.h"
#include <QString>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace plugin {

/**
 * @brief 插件加载器
 * 
 * 负责动态库的加载和卸载
 */
class PluginLoader {
public:
    /**
     * @brief 构造加载器
     * @param filePath 插件文件路径
     */
    explicit PluginLoader(const QString& filePath);
    
    ~PluginLoader();
    
    // 禁用拷贝
    PluginLoader(const PluginLoader&) = delete;
    PluginLoader& operator=(const PluginLoader&) = delete;
    
    // ========================================================================
    // 加载/卸载
    // ========================================================================
    
    /**
     * @brief 加载插件
     * @return true 表示加载成功
     */
    bool load();
    
    /**
     * @brief 卸载插件
     */
    void unload();
    
    /**
     * @brief 检查是否已加载
     */
    bool isLoaded() const { return handle_ != nullptr; }
    
    // ========================================================================
    // 插件访问
    // ========================================================================
    
    /**
     * @brief 创建插件实例
     * @return 插件实例，失败返回 nullptr
     */
    std::unique_ptr<IPlugin> createPlugin();
    
    /**
     * @brief 获取插件文件路径
     */
    const QString& filePath() const { return filePath_; }
    
    /**
     * @brief 获取最后错误信息
     */
    QString lastError() const { return lastError_; }

private:
    /**
     * @brief 获取导出函数
     */
    template<typename T>
    T getExportedFunction(const char* name);
    
    /**
     * @brief 获取错误信息
     */
    static QString getSystemError();

private:
    QString filePath_;
    void* handle_{nullptr};
    QString lastError_;
};

// ============================================================================
// 模板实现
// ============================================================================

template<typename T>
T PluginLoader::getExportedFunction(const char* name) {
    if (!handle_) {
        return nullptr;
    }
    
#ifdef _WIN32
    FARPROC proc = GetProcAddress(static_cast<HMODULE>(handle_), name);
    return reinterpret_cast<T>(proc);
#else
    void* proc = dlsym(handle_, name);
    return reinterpret_cast<T>(proc);
#endif
}

} // namespace plugin

#endif // PLUGIN_PLUGIN_LOADER_H
