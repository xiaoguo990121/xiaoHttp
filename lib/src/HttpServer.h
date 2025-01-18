/**
 * @file HttpServer.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoNet/net/TcpServer.h>
#include <xiaoNet/utils/NonCopyable.h>
#include <functional>
#include <string>
#include <vector>
#include "impl_forwards.h"

struct CallbackParamPack;

namespace xiaoHttp
{
    class ControllerBinderBase;

    class HttpServer : xiaoNet::NonCopyable
    {
    public:
        HttpServer(xiaoNet::EventLoop *loop,
                   const xiaoNet::InetAddress &listenAddr,
                   std::string name);

        ~HttpServer();

        void setIoLoops(const std::vector<xiaoNet::EventLoop *> &ioLoops)
        {
            server_.setIoLoops(ioLoops);
        }

        void start();

        void stop();

        void enableSSL(xiaoNet::TLSPolicyPtr policy)
        {
            server_.enableSSL(std::move(policy));
        }

        const xiaoNet::InetAddress &address() const
        {
            return server_.address();
        }

    private:
        friend class HttpInternalForwardHelper;

        static void onConnection(const xiaoNet::TcpConnectionPtr &conn);
        static void onMessage(const xiaoNet::TcpConnectionPtr &,
                              xiaoNet::MsgBuffer *);
        static void onRequests(const xiaoNet::TcpConnectionPtr &,
                               const std::vector<HttpRequestImplPtr> &,
                               const std::shared_ptr<HttpRequestParser> &);

        struct HttpRequestParamPack
        {
            std::shared_ptr<ControllerBinderBase> binderPtr;
            std::function<void(const HttpResponsePtr &)> callback;
        };

        struct WsRequestParamPack
        {
            std::shared_ptr<ControllerBinderBase> binderPtr;
            std::function<void(const HttpResponsePtr &)> callback;
            WebSocketConnectionImplPtr wsConnPtr;
        };

        static void onHttpRequest(const HttpRequestImplPtr &,
                                  std::function<void(const HttpResponsePtr &)> &&);
        static void httpRequestRouting(
            const HttpRequestImplPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);

        static void httpRequestHandling(
            const HttpRequestImplPtr &req,
            std::shared_ptr<ControllerBinderBase> &&binderPtr,
            std::function<void(const HttpResponsePtr &)> &&callback);

        // Websocket request handling steps
        static void onWebsocketRequest(
            const HttpRequestImplPtr &,
            std::function<void(const HttpResponsePtr &)> &&,
            WebSocketConnectionImplPtr &&);
        static void websocketRequestRouting(
            const HttpRequestImplPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            WebSocketConnectionImplPtr &&wsConnPtr);
        static void websocketRequestHandling(
            const HttpRequestImplPtr &req,
            std::shared_ptr<ControllerBinderBase> &&binderPtr,
            std::function<void(const HttpResponsePtr &)> &&callback,
            WebSocketConnectionImplPtr &&wsConnPtr);

        // Http/Websocket shared handling steps
        template <typename Pack>
        static void requestPostRouting(const HttpRequestImplPtr &req, Pack &&pack);
        template <typename Pack>
        static void requestPassFilters(const HttpRequestImplPtr &req, Pack &&pack);
        template <typename Pack>
        static void requestPreHandling(const HttpRequestImplPtr &req, Pack &&pack);

        // Response buffering and sending
        static void handleResponse(
            const HttpResponsePtr &response,
            const std::shared_ptr<CallbackParamPack> &paramPack,
            bool *respReadyPtr);
        static void sendResponse(const xiaoNet::TcpConnectionPtr &,
                                 const HttpResponsePtr &,
                                 bool isHeadMethod);
        static void sendResponses(
            const xiaoNet::TcpConnectionPtr &conn,
            const std::vector<std::pair<HttpResponsePtr, bool>> &responses,
            xiaoNet::MsgBuffer &buffer);

        xiaoNet::TcpServer server_;
    };

    class HttpInternalForwardHelper
    {
    public:
        static void forward(const HttpRequestImplPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback)
        {
            return HttpServer::onHttpRequest(req, std::move(callback));
        }
    };
}