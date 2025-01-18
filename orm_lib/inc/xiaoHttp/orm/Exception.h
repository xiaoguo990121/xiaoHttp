/**
 * @file Exception.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <functional>
#include <stdexcept>
#include <string>

namespace xiaoHttp
{
    namespace orm
    {

        class XIAOHTTP_EXPORT XiaoHttpDbException
        {
        public:
            virtual ~XiaoHttpDbException() noexcept
            {
            }

            virtual const std::exception &base() const noexcept
            {
                static std::exception except;
                return except;
            }
        };

        class Failure : public XiaoHttpDbException, public std::runtime_error
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit Failure(const std::string &);
        };
    }
}
