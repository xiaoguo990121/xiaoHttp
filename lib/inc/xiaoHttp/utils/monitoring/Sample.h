/**
 * @file Sample.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once

#include <xiaoLog/Date.h>
#include <vector>
#include <string>

namespace xiaoHttp
{
    namespace monitoring
    {
        /**
         * @brief This class is used to collect samples for a metric.
         *
         */
        struct Sample
        {
            double value{0};
            xiaoLog::Date timestamp{0};
            std::string name;
            std::vector<std::pair<std::string, std::string>> exLabels;
        };
    }
}
