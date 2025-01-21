/**
 * @file HttpConstraint.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoHttp/HttpTypes.h>
#include <string>

namespace xiaoHttp
{
    namespace internal
    {
        enum class ConstraintType
        {
            None,
            HttpMethod,
            HttpFilter
        };

        class HttpConstraint
        {
        public:
            HttpConstraint(HttpMethod method)
                : type_(ConstraintType::HttpMethod), method_(method)
            {
            }

            HttpConstraint(const std::string &filterName)
                : type_(ConstraintType::HttpFilter), filterName_(filterName)
            {
            }

            HttpConstraint(const char *filterName)
                : type_(ConstraintType::HttpFilter), filterName_(filterName)
            {
            }

            ConstraintType type() const
            {
                return type_;
            }

            HttpMethod getHttpMethod() const
            {
                return method_;
            }

            const std::string &getMiddlewareName() const
            {
                return middlewareName_;
            }

        private:
            ConstraintType type_{ConstraintType::None};
            HttpMethod method_{HttpMethod::Invalid};
            std::string middlewareName_;
        };
    }
}
