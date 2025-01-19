/**
 * @file ConfigAdapter.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#pragma once
#include <json/json.h>
#include <vector>
#include <string>
#include <memory>

namespace xiaoHttp
{
    class ConfigAdapter
    {
    public:
        virtual ~ConfigAdapter() = default;
        virtual Json::Value getJson(const std::string &content) const
            noexcept(false) = 0;
        virtual std::vector<std::string> getExtensions() const = 0;
    };

    using ConfigAdapterPtr = std::shared_ptr<ConfigAdapter>;
}