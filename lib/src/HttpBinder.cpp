/**
 * @file HttpBinder.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#include <xiaoHttp/HttpBinder.h>

namespace xiaoHttp
{
    namespace internal
    {
        void handleException(const std::exception &e,
                             const HttpRequestPtr &req,
                             std::function<void(const HttpResponsePtr &)> &&callback)
        {
            return;
        }
    }
}