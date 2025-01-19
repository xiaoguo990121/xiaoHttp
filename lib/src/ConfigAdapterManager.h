/**
 * @file ConfigAdapterManager.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#pragma once
#include "ConfigAdapter.h"
#include <map>

namespace xiaoHttp
{
    class ConfigAdapterManager
    {
    public:
        static ConfigAdapterManager &instance();
        Json::Value getJson(const std::string &content, std::string ext) const
            noexcept(false);

    private:
        ConfigAdapterManager();
        std::map<std::string, ConfigAdapterPtr> adapters_;
    };
}