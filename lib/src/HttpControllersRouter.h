/**
 * @file HttpControllersRouter.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include "impl_forwards.h"
#include "ControllerBinderBase.h"
#include <xiaoNet/utils/NonCopyable.h>
#include <memory>
#include <regex>
#include <unordered_map>
#include <vector>
#include <string>

namespace xiaoHttp
{
    class HttpControllerBinder;
    class HttpSimpleControllerBinder;
    class WebsocketControllerBinder;

    class HttpControllersRouter : public xiaoNet::NonCopyable
    {
    public:
        static HttpControllersRouter &instance()
        {
            static HttpControllersRouter inst;
            return inst;
        }

        void init(const std::vector<xiaoNet::EventLoop *> &ioLoops);
        void reset();

        void registerHttpSimpleController(
            const std::string &pathName,
            const std::string &ctrlName,
            const std::vector<internal::HttpConstraint> &filtersAndMethods);
        void registerWebSocketController(
            const std::string &pathName,
            const std::string &ctrlName,
            const std::vector<internal::HttpConstraint> &filtersAndMethods);
        void addHttpPath(const std::string &path,
                         const internal::HttpBinderBasePtr &binder,
                         const std::vector<HttpMethod> &validMethods,
                         const std::vector<std::string> &filters,
                         const std::string &handlerName = "");
        void addHttpRegex(const std::string &regExp,
                          const internal::HttpBinderBasePtr &binder,
                          const std::vector<HttpMethod> &validMethods,
                          const std::vector<std::string> &filters,
                          const std::string &handlerName = "");
        RouteResult route(const HttpRequestImplPtr &req);
        RouteResult routeWs(const HttpRequestImplPtr &req);
        std::vector<HttpHandlerInfo> getHandlersInfo() const;

    private:
        void addRegexCtrlBinder(
            const std::shared_ptr<HttpControllerBinder> &binderPtr,
            const std::string &pathPattern,
            const std::string &pathParameterPattern,
            const std::vector<HttpMethod> &methods);

        struct SimpleControllerRouterItem
        {
            std::shared_ptr<HttpSimpleControllerBinder> binders_[Invalid]{nullptr};
        };

        struct HttpControllerRouterItem
        {
            std::string pathParameterPattern_;
            std::string pathPattern_;
            std::regex regex_;
            std::shared_ptr<HttpControllerBinder> binders_[Invalid]{nullptr};
        };

        struct WebSocketControllerRouterItem
        {
            std::shared_ptr<WebSocketControllerBinder> binders_[Invalid]{nullptr};
        };

        std::unordered_map<std::string, SimpleControllerRouterItem> simpleCtrlMap_;
        std::unordered_map<std::string, HttpControllerRouterItem> ctrlMap_;
        std::vector<HttpControllerRouterItem> ctrlVector_; // for regexp path
        std::unordered_map<std::string, WebSocketControllerRouterItem> wsCtrlMap_;
    };
}