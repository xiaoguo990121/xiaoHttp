/**
 * @file xiaoHttp_callbacks.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoHttp/HttpTypes.h>
#include <functional>
#include <memory>

namespace xiaoHttp
{
    class HttpResponse;
    using HttpResponsePtr = std::shared_ptr<HttpResponse>;
    class HttpRequest;
    using HttpRequestPtr = std::shared_ptr<HttpRequest>;
    using AdviceCallback = std::function<void(const HttpResponsePtr &)>;
    using AdviceChainCallback = std::function<void()>;
    using AdviceStartSessionCallback = std::function<void(const std::string &)>;
    using AdviceDestorySessionCallback = std::function<void(const std::string &)>;
    using FilterCallback = std::function<void(const HttpResponsePtr &)>;
    using FilterChainCallback = std::function<void()>;
    using HttpReqCallback = std::function<void(ReqResult, const HttpResponsePtr &)>;
}