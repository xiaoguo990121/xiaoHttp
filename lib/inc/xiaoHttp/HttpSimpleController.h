/**
 * @file HttpSimpleController.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 *
 */

#pragma once

#include <xiaoHttp/DrObject.h>

#define PATH_LIST_BEGIN           \
    static void initPathRouting() \
    {
#define PATH_ADD(path, ...) registerSelf__(path, {__VA_ARGS__})
#define PATH_LIST_END }

namespace xiaoHttp
{
    /**
     * @brief The abstract base class for HTTP simple controllers.
     *
     */
    class HttpSimpleControllerBase : public virtual DrObjectBase
    {
    public:
        /**
         * @brief The function is called when a HTTP requests is routed to the
         * controller.
         *
         * @param req
         * @param callback
         */
        virtual void asyncHandleHttpRequest(
            const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) = 0;

        virtual ~HttpSimpleControllerBase()
        {
        }
    };

    template <typename T, bool AutoCreation = true>
    class HttpSimpleController : public DrObject<T>, public HttpSimpleControllerBase
    {
    public:
        static const bool isAutoCreation = AutoCreation;

        virtual ~HttpSimpleController()
        {
        }

    protected:
        HttpSimpleController()
        {
        }

        static void registerSelf__(
            const std::string &path,
            const std::vector<internal::HttpConstraint> &constraints)
        {
            LOG_TRACE << "register simple controller("
                      << HttpSimpleController<T, AutoCreation>::classTypeName()
                      << ") on path:" << path;
            app().registerHttpSimpleController(
                path,
                HttpSimpleController<T, AutoCreation>::classTypeName(),
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
    typename HttpSimpleController<T, AutoCreation>::pathRegistrator
        HttpSimpleController<T, AutoCreation>::registrator_;
}