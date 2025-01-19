/**
 * @file HttpControllerBinder.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 *
 */

#pragma once

#include "ControllerBinderBase.h"
#include "HttpRequestImpl.h"
#include "WebSocketConnectionImpl.h"
#include <xiaoHttp/HttpBinder.h>

namespace xiaoHttp
{
    class HttpSimpleControllerBinder : public ControllerBinderBase
    {
    public:
        void handleRequest(
            const HttpRequestImplPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) const override;

        std::shared_ptr<HttpSimpleControllerBase> controller_;
    };
}