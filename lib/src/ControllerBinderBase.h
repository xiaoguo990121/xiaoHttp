/**
 * @file ControllerBinderBase.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 *
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <xiaoHttp/IOThreadStorage.h>
#include <xiaoHttp/HttpResponse.h>
#include "HttpRequestImpl.h"

namespace xiaoHttp
{
    class HttpFilterBase;

    /**
     * @brief A component to associate router class and controller class
     *
     */
    struct ControllerBinderBase
    {
        std::string handlerName_;
        std::vector<std::string> filterNames_;
        std::vector<std::shared_ptr<HttpFilterBase>> filters_;
        IOThreadStorage<HttpResponsePtr> responseCache_;
        std::shared_ptr<std::string> corsMethods_;
        bool isCORS_{false};

        virtual ~ControllerBinderBase() = default;
        virtual void handleRequest(
            const HttpRequestImplPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const = 0;
    };

    struct RouteResult
    {
        enum
        {
            Success,
            MethodNotAllowed,
            NotFound
        } result;

        std::shared_ptr<ControllerBinderBase> binderPtr;
    }
}