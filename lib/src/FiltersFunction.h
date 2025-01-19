/**
 * @file FiltersFunction.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#pragma once

#include "impl_forwards.h"
#include <memory>
#include <string>
#include <vector>

namespace xiaoHttp
{
    namespace filters_function
    {
        std::vector<std::shared_ptr<HttpFilterBase>> createFilters(
            const std::vector<std::string> &filterNames);

        void doFilters(const std::vector<std::shared_ptr<HttpFilterBase>> &filters,
                       const HttpRequestImplPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback);
    }
}