/**
 * @file FiltersFunction.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#include "FiltersFunction.h"
#include "HttpRequestImpl.h"
#include <xiaoHttp/HttpFilter.h>

namespace xiaoHttp
{
    namespace filters_function
    {
        static void doFilterChains(
            const std::vector<std::shared_ptr<HttpFilterBase>> &filters,
            size_t index,
            const HttpRequestImplPtr &req,
            std::shared_ptr<const std::function<void(const HttpResponsePtr &)>>
                &&callbackPtr)
        {
            if (index < filters.size())
            {
                auto &filter = filters[index];
                filter->doFilter(
                    req,
                    [callbackPtr](const HttpResponsePtr &resp)
                    {
                        (*callbackPtr)(resp);
                    },
                    [index, req, callbackPtr, &filters]() mutable
                    {
                        auto ioLoop = req->getLoop();
                        if (ioLoop && !ioLoop->isInLoopThread())
                        {
                            ioLoop->queueInLoop(
                                [&filters,
                                 index,
                                 req,
                                 callbackPtr = std::move(callbackPtr)]() mutable
                                {
                                    doFilterChains(filters,
                                                   index + 1,
                                                   req,
                                                   std::move(callbackPtr));
                                });
                        }
                        else
                        {
                            doFilterChains(filters,
                                           index + 1,
                                           req,
                                           std::move(callbackPtr));
                        }
                    });
            }
            else
            {
                (*callbackPtr)(nullptr);
            }
        }

        std::vector<std::shared_ptr<HttpFilterBase>> createFilters(
            const std::vector<std::string> &filterNames)
        {
            std::vector<std::shared_ptr<HttpFilterBase>> filters;
            for (auto const &filter : filterNames)
            {
                auto object_ = DrClassMap::getSingleInstance(filter);
                auto filter_ = std::dynamic_pointer_cast<HttpFilterBase>(object_);
                if (filter_)
                    filters.push_back(filter_);
                else
                {
                    LOG_ERROR << "filter " << filter << " not found";
                }
            }
            return filters;
        }

        void doFilters(const std::vector<std::shared_ptr<HttpFilterBase>> &filters,
                       const HttpRequestImplPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback)
        {
            auto callbackPtr =
                std::make_shared<std::decay_t<decltype(callback)>>(std::move(callback));
            doFilterChains(filters, 0, req, std::move(callbackPtr));
        }

    }
}