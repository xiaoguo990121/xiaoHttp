/**
 * @file ConfigLoader.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 *
 */

#pragma once

#include <json/json.h>
#include <string>
#include <xiaoNet/utils/NonCopyable.h>

namespace xiaoHttp
{
    class ConfigLoader : public xiaoNet::NonCopyable
    {
    public:
        explicit ConfigLoader(const std::string &configFile) noexcept(false);
        explicit ConfigLoader(const Json::Value &data);
        explicit ConfigLoader(Json::Value &&data);
        ~ConfigLoader();

        const Json::Value &jsonValue() const
        {
            return configJsonRoot_;
        }

        void load() noexcept(false);

    private:
        std::string configFile_;
        Json::Value configJsonRoot_;
    };
}