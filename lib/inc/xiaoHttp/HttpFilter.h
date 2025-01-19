/**
 * @file HttpFilter.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#pragma once

#include <xiaoHttp/DrObject.h>
#include <xiaoHttp/xiaoHttp_callbacks.h>
#include <xiaoHttp/HttpRequest.h>
#include <xiaoHttp/HttpResponse.h>
#include <memory>

#ifdef __cpp_impl_coroutine
#include <xiaoHttp/utils/coroutine.h>
#endif

namespace xiaoHttp
{
    /**
     * @brief The abstract base class for filters
     *
     */
    class XIAOHTTP_EXPORT HttpFilterBase : public virtual DrObjectBase
    {
    public:
        /**
         * @brief This method is an asynchronous interface, user should return the result
         * via 'FilterCallback' or 'FilterChainCallback'.
         *
         * @param req is the request object processed by the filter
         * @param fcb if this is called, the response object is send to the client
         * by the callback, and doFilter methods of next filters and the handler
         * registered on the path are not called anymore.
         * @param fccb if this callback is called, the next filter's doFilter method
         * or the handler registered on the path is called.
         */
        virtual void doFilter(const HttpRequestPtr &req,
                              FilterCallback &&fcb,
                              FilterChainCallback &&fccb) = 0;
        ~HttpFilterBase() override = default;
    };

    /**
     * @brief The reflection base class template for filters
     *
     * @tparam T The type of the implementation class
     * @tparam AutoCreation The flag for automatically creating, user can set this
     * flag to false for classes that have nodefault constructors.
     */
    template <typename T, bool AutoCreation = true>
    class HttpFilter : public DrObject<T>, public HttpFilterBase
    {
    public:
        static constexpr bool isAutoCreation{AutoCreation};
        ~HttpFilter() override = default;
    };

    namespace internal
    {
        XIAOHTTP_EXPORT void handleException(
            const std::exception &,
            const HttpRequestPtr &,
            std::function<void(const HttpResponsePtr &)> &&);
    }

#ifdef __cpp_impl_coroutine
    template <typename T, bool AutoCreation = true>
    class HttpCoroFilter : public DrObject<T>, public HttpFilterBase
    {
    public:
        static constexpr bool isAutoCreation{AutoCreation};
        ~HttpCoroFilter() override = default;

        void doFilter(const HttpRequestPtr &req,
                      FilterCallback &&fcb,
                      FilterChainCallback &&fccb) final
        {
            xiaoHttp::async_run([this,
                                 req,
                                 fcb = std::move(fcb),
                                 fccb = std::move(fccb)]() mutable -> xiaoHttp::Task<>
                                {
                                    HttpResponsePtr resp;
                                    try
                                    {
                                        resp = co_await doFilter(req);
                                    }
                                    catch (const std::exception &ex)
                                    {
                                        internal::handleException(ex, req, std::move(fcb));
                                        co_return;
                                    }
                                    catch(...)
                                    {
                                        LOG_ERROR << "Exception not derived from std::exception";
                                        co_return;
                                    }
                                    
                                    if(resp)
                                    {
                                        fcb(resp);
                                    } 
                                    else
                                    {
                                        fccb();
                                    } });
        }
        virtual Task<HttpResponsePtr> doFilter(const HttpRequestPtr &req) = 0;
    };
#endif
}