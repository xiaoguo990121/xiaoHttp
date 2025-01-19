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

    using WebSocketControllerBasePtr = std::shared_ptr<WebSocketControllerBase>;

    template <typename T, bool AutoCreation = true>
    class WebSocketController : public DrObject<T>, public WebSocketControllerBase
    {
    public:
        static const bool isAutoCreation = AutoCreation;

        virtual ~WebSocketController()
        {
        }

    protected:
        WebSocketController()
        {
        }

        static void registerSelf__(
            const std::string &path,
            const std::vector<internal::HttpConstraint> &constraints)
        {
            LOG_TRACE << "register websocket controller("
                      << WebSocketController<T, AutoCreation>::classTypeName()
                      << ") on path:" << path;
            app().registerWebSocketController(
                path,
                WebSocketController<T, AutoCreation>::classTypeName(),
                constraints);
        }

        static void registerSelfRegex__(
            const std::string &regExp,
            const std::vector<internal::HttpConstraint> &constraints)
        {
            LOG_TRACE << "register websocket controller("
                      << WebSocketController<T, AutoCreation>::classTypeName()
                      << ") on regExp:" << regExp;
            app().registerWebSocketControllerRegex(
                regExp,
                WebSocketController<T, AutoCreation>::classTypeName(),
                constraints);
        }

    private:
        class pathRegistrator
        {
        public:
            pathRegistrator()
            {
                if (AutoCreation)
                {
                    T::initPathRouting();
                }
            }
        };

        friend pathRegistrator;
        static pathRegistrator registrator_;

        virtual void *touch()
        {
            return &registrator_;
        }
    };

    template <typename T, bool AutoCreation>
    typename WebSocketController<T, AutoCreation>::pathRegistrator
        WebSocketController<T, AutoCreation>::registerator_;
}