/**
 * @file PluginsManager.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once
#include <xiaoHttp/plugins/Plugin.h>
#include <map>

namespace xiaoHttp
{
    using PluginBasePtr = std::shared_ptr<PluginBase>;

    class PluginsManager : xiaoNet::NonCopyable
    {
    public:
        void initializeAllPlugins(
            const Json::Value &configs,
            const std::function<void(PluginBase *)> &forEachCallback);

        PluginBase *getPlugin(const std::string &pluginName);

        std::shared_ptr<PluginBase> getSharedPlugin(const std::string &pluginName);

        ~PluginsManager();

    private:
        void createPlugin(const std::string &pluginName);
        std::map<std::string, PluginBasePtr> pluginsMap_;
        std::vector<PluginBase *> initializedPlugins_;
    }
}