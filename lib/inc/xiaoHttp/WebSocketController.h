/**
 * @file WebSocketController.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#pragma once

#include <xiaoHttp/DrObject.h>
#include <xiaoHttp/HttpTypes.h>
#include <xiaoHttp/HttpAppFramework.h>
#include <xiaoHttp/WebSocketConnection.h>

#define WS_PATH_LIST_BEGIN        \
    static void initPathRouting() \
    {
#define WS_PATH_ADD(path, ...) registerSelf__(path, {__VA_ARGS__})
#define WS_PATH_LIST_END }

namespace xiaoHttp
{
    class WebSocketControllerBase : public virtual DrObjectBase
    {
    public:
        // This function is called when a new message is received
        virtual void handleNewMessage(const WebSocketConnectionPtr &,
                                      std::string &&,
                                      const WebSocketMessageType &) = 0;
        // This function is called after a new connection of WebSocket is established.
        virtual void handleNewConnection(const HttpRequestPtr &,
                                         const WebSocketConnectionPtr &) = 0;
        // This function is called after a WebSocket connection is closed
        virtual void handleConnectionClosed(const WebSocketConnectionPtr &) = 0;

        virtual ~WebSocketControllerBase()
        {
        }
    };
}