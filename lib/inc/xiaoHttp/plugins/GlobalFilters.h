/**
 * @file GlobalFilters.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once

#include <xiaoHttp/plugins/Plugin.h>
#include <regex>
#include <vector>
#include <memory>
#include <xiaoHttp/HttpFilter.h>

namespace xiaoHttp
{
    namespace plugin
    {
        /**
         * @brief This plugin is used to add global filters to all HTTP requests.
         * The json configuration is as follows:
         *
         * @code
           {
                "name": "drogon::plugin::GlobalFilters",
                "dependencies": [],
                "config": {
                    // filters: the list of global filter names.
                    "filters": [
                        "FilterName1", "FilterName2",...
                    ],
                    // exempt: exempt must be a string or string array, regular
         expressions for
                    // URLs that don't have to be filtered.
                    "exempt": [
                        "^/static/.*\\.css", "^/images/.*",...
                    ]
                }
           }
           @endcode
         *
         */
        class XIAOHTTP_EXPORT GlobalFilters
        {
        public:
            GlobalFilters()
            {
            }

            void initAndStart(const Json::Value &config) override;
            void shutdown() override;

        private:
            std::vector<std::shared_ptr<xiaoHttp::HttpFilterBase>> filters_;
            std::regex exemptPegex_;
            bool regexFlag_{false};
        };
    }
}