/**
 * @file ConfigAdapterManager.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#include "ConfigAdapterManager.h"
#include "JsonConfigAdapter.h"
#include "YamlConfigAdapter.h"
#include <algorithm>

using namespace xiaoHttp;
#define REGISTER_CONFIG_ADAPTER(adapter)                                    \
    {                                                                       \
        auto adapterPtr = std::make_shared<adapter>();                      \
        auto exts = adapterPtr->getExtensions();                            \
        for (auto ext : exts)                                               \
        {                                                                   \
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); \
            adapters_[ext] = adapterPtr;                                    \
        }                                                                   \
    }

ConfigAdapterManager &ConfigAdapterManager::instance()
{
    static ConfigAdapterManager instance;
    return instance;
}

Json::Value ConfigAdapterManager::getJson(const std::string &content,
                                          std::string ext) const noexcept(false)
{
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    auto it = adapters_.find(ext);
    if (it == adapters_.end())
    {
        throw std::runtime_error("No valid parser for this config file!");
    }
    return it->second->getJson(content);
}

ConfigAdapterManager::ConfigAdapterManager()
{
    REGISTER_CONFIG_ADAPTER(JsonConfigAdapter);
    REGISTER_CONFIG_ADAPTER(YamlConfigAdapter);
}