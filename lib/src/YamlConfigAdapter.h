/**
 * @file YamlConfigAdapter.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#pragma once
#include "ConfigAdapter.h"

namespace xiaoHttp
{
    class YamlConfigAdapter : public ConfigAdapter
    {
    public:
        YamlConfigAdapter() = default;
        ~YamlConfigAdapter() override = default;
        Json::Value getJson(const std::string &content) const
            noexcept(false) override;
        std::vector<std::string> getExtensions() const override;
    };
}