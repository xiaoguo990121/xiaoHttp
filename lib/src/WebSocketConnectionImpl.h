/**
 * @file WebSocketConnectionImpl.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#pragma once

#include "impl_forwards.h"
#include <xiaoHttp/WebSocketConnection.h>
#include <string_view>
#include <xiaoNet/utils/NonCopyable.h>
#include <xiaoNet/net/TcpConnection.h>

namespace xiaoHttp
{
    class WebSocketConnectionImpl;
    using WebSocketConnectionImplPtr = std::shared_ptr<WebSocketConnectionImpl>;

    class WebSocketMessageParser
    {
    public:
        bool parse(xiaoNet::MsgBuffer *buffer);

        bool gotAll(std::string &message, WebSocketMessageType &type)
        {
            assert(message.empty());
            if (!gotAll_)
                return false;
            message.swap(message_);
            type = type_;
            return true;
        }

    private:
        std::string message_;
        WebSocketMessageType type_;
        bool gotAll_{false};
    };

    class WebSocketConnectionImpl final
        : public WebSocketConnection,
          public std::enable_shared_from_this<WebSocketConnectionImpl>,
          public xiaoNet::NonCopyable
    {
    private:
        xiaoNet::TcpConnectionPtr tcpConnectionPtr_;
        xiaoNet::InetAddress localAddr_;
        xiaoNet::InetAddress peerAddr_;
        bool isServer_{true};
        WebSocketMessageParser parser_;
        xiaoNet::TimerId pingTimerId_{xiaoNet::InvalidTimerId};
        std::vector<uint32_t> masks_;
        std::atomic<bool> usingMask_;

        std::function<void(std::string &&,
                           const WebSocketConnectionImplPtr &,
                           const WebSocketMessageType &)>
            messageCallback_ = [](std::string &&,
                                  const WebSocketConnectionImplPtr &,
                                  const WebSocketMessageType &) {};
        std::function<void(const WebSocketConnectionImplPtr &)> closeCallback_ =
            [](const WebSocketConnectionImplPtr &) {};
        void sendWsData(const char *msg, uint64_t len, unsigned char opcode);
        void disablePingInLoop();
        void setPingMessageInLoop(std::string &&message,
                                  const std::chrono::duration<double> &interval);
    };
}