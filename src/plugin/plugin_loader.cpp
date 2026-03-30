#include "plugin/plugin_loader.h"
#include <QLibrary>

namespace plugin {

// ============================================================================
// PluginLoader 实现
// ============================================================================

PluginLoader::PluginLoader(const QString& filePath)
    : filePath_(filePath) {
}

PluginLoader::~PluginLoader() {
    unload();
}

bool PluginLoader::load() {
    if (handle_) {
        return true;  // 已经加载
    }
    
#ifdef _WIN32
    HMODULE handle = LoadLibraryW(filePath_.toStdWString().c_str());
    if (!handle) {
        lastError_ = getSystemError();
        return false;
    }
    handle_ = handle;
#else
    void* handle = dlopen(filePath_.toStdString().c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        lastError_ = QString::fromUtf8(dlerror());
        return false;
    }
    handle_ = handle;
#endif
    
    return true;
}

void PluginLoader::unload() {
    if (!handle_) {
        return;
    }
    
#ifdef _WIN32
    FreeLibrary(static_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif
    
    handle_ = nullptr;
    lastError_.clear();
}

std::unique_ptr<IPlugin> PluginLoader::createPlugin() {
    if (!handle_) {
        if (!load()) {
            return nullptr;
        }
    }
    
    // 获取工厂函数
    auto factoryFunc = getExportedFunction<GetPluginFactoryFunc>("getPluginFactory");
    if (!factoryFunc) {
        lastError_ = "Failed to get getPluginFactory function";
        return nullptr;
    }
    
    // 检查 API 版本
    auto versionFunc = getExportedFunction<GetPluginApiVersionFunc>("getPluginApiVersion");
    if (versionFunc) {
        int apiVersion = versionFunc();
        if (apiVersion != PLUGIN_API_VERSION) {
            lastError_ = QString("API version mismatch: plugin=%1, expected=%2")
                .arg(apiVersion).arg(PLUGIN_API_VERSION);
            return nullptr;
        }
    }
    
    // 创建插件实例
    IPluginFactory* factory = factoryFunc();
    if (!factory) {
        lastError_ = "Factory returned null";
        return nullptr;
    }
    
    return std::unique_ptr<IPlugin>(factory->createPlugin());
}

QString PluginLoader::getSystemError() {
#ifdef _WIN32
    DWORD errorCode = GetLastError();
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );
    
    QString result = QString::fromUtf8(messageBuffer, static_cast<int>(size));
    LocalFree(messageBuffer);
    return result;
#else
    return QString::fromUtf8(dlerror());
#endif
}

} // namespace plugin
