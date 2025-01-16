/**
 * @file impl_forwards.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 *
 */

#pragma once

#include <memory>
#include <functional>

namespace xiaoHttp
{
    class HttpRequest;
    using HttpRequestPtr = std::shared_ptr<HttpRequest>;
    class HttpResponse;
    using HttpResponsePtr = std::shared_ptr<HttpResponse>;
    class Cookie;
    class Session;
    using SessionPtr = std::shared_ptr<Session>;

    class HttpRequestImpl;
    using HttpRequestImplPtr = std::shared_ptr<HttpRequestImpl>;

    class WebSocketConnectionImpl;
    using WebSocketConnectionImplPtr = std::shared_ptr<WebSocketConnectionImpl>;
}