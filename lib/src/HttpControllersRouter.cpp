/**
 * @file HttpControllersRouter.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 *
 */

#include "HttpControllersRouter.h"
#include "HttpControllerBinder.h"
#include "HttpRequestImpl.h"
#include "HttpResponseImpl.h"
#include "HttpAppFrameworkImpl.h"
#include "FiltersFunction.h"
#include <xiaoHttp/HttpSimpleController.h>
#include <xiaoHttp/WebSocketConnection.h>
#include <algorithm>

using namespace xiaoHttp;

void HttpControllersRouter::init(
    const std::vector<xiaoNet::EventLoop *> &ioLoops)
{
    auto initFiltersAndCorsMethods = [](const auto &item)
    {
        auto corsMethods = std::make_shared<std::string>("OPTIONS,");
        for (size_t i = 0; i < Invalid; ++i)
        {
            auto &binder = item.binders_[i];
            if (binder)
            {
                binder->filters_ =
                    filters_function::createFilters(binder->filterNames_);
                binder->corsMethods_ = corsMethods;
                if (binder->isCORS_)
                {
                    if (i == Get)
                    {
                        corsMethods->append("GET,HEAD,");
                    }
                    else if (i != Options)
                    {
                        corsMethods->append(to_string_view((HttpMethod)i));
                        corsMethods->append(",");
                    }
                }
            }
        }
        corsMethods->pop_back(); // remove last comma
    };

    for (auto &iter : simpleCtrlMap_)
    {
        initFiltersAndCorsMethods(iter.second);
    }

    for (auto &iter : wsCtrlMap_)
    {
        initFiltersAndCorsMethods(iter.second);
    }

    for (auto &iter : ctrlVector_)
    {
        router.regex_ = std::regex(router.pathParameterPattern_,
                                   std::regex_constants::icase);
        initFiltersAndCorsMethods(router);
    }

    for (auto &p : ctrlMap_)
    {
        auto &router = p.second;
        router.regex_ = std::regex(router.pathParameterPattern_,
                                   std::regex_constants::icase);
        initFiltersAndCorsMethods(router);
    }
}

void HttpControllersRouter::reset()
{
    simpleCtrlMap_.clear();
    ctrlMap_.clear();
    ctrlVector_.clear();
    wsCtrlMap_.clear();
}

std::vector<HttpHandlerInfo> HttpControllersRouter::getHandlersInfo() const
{
    std::vector<HttpHandlerInfo> ret;
    auto gatherInfo = [&ret](const std::string &path, const auto &item)
    {
        for (size_t i = 0; i < Invalid; ++i)
        {
            if (item.binders_[i])
            {
                std::string description;
                if constexpr (std::is_same_v<std::decay_t<decltype(item)>,
                                             SimpleControllerRouterItem>)

                {
                    description = std::string("HttpSimpleController: ") +
                                  item.binders_[i]->handlerName_;
                }
                else if constexpr (std::is_same_v<
                                       std::decay_t<decltype(item)>,
                                       WebSocketControllerRouterItem>)
                {
                    description = std::string("WebsocketController: ") +
                                  item.binders_[i]->handlerName_;
                }
                else
                {
                    description =
                        item.binders_[i]->handlerName_.empty()
                            ? std::string("Handler: ") +
                                  item.binders_[i]->binderPtr_->handlerName()
                            : std::string("HttpController: ") +
                                  item.binders_[i]->handlerName_;
                }
                ret.emplace_back(path, (HttpMethod)i, std::move(description));
            }
        }
    };

    for (auto &[path, item] : simpleCtrlMap_)
    {
        gatherInfo(path, item);
    }
    for (auto &item : ctrlVector_)
    {
        gatherInfo(item.pathPattern_, item);
    }
    for (auto &[key, item] : ctrlMap_)
    {
        gatherInfo(item.pathPattern_, item);
    }
    for (auto &[path, item] : wsCtrlMap_)
    {
        gatherInfo(path, item);
    }
    return ret;
}

template <typename Binder, typename RouterItem>
static void addCtrlBinderToRouterItem(const std::shared_ptr<Binder> &binderPtr,
                                      RouterItem &router,
                                      const std::vector<HttpMethod> &methods)
{
    if (!methods.empty())
    {
        for (const auto &method : methods)
        {
            router.binders_[method] = binderPtr;
            if (method == Options)
            {
                binderPtr->isCORS_ = true;
            }
        }
    }
    else
    {
        binderPtr->isCORS_ = true;
        for (int i = 0; i < Invalid; ++i)
        {
            router.binders_[i] = binderPtr;
        }
    }
}

struct SimpleControllerProcessResult
{
    std::string lowerPath;
    std::vector<HttpMethod> validMethods;
    std::vector<std::string> filters;
};