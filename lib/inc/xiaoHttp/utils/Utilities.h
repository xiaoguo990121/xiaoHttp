/**
 * @file Utilities.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <xiaoLog/Date.h>

namespace xiaoHttp
{
    namespace internal
    {

    }

    namespace utils
    {
        /// Get the http full date string
        /**
         * rfc2616-3.3.1
         * Full Date format(RFC 822)
         * like this:
         * @code
           Sun, 06 Nov 1994 08:49:37 GMT
           Wed, 12 Sep 2018 09:22:40 GMT
           @endcode
         */
        XIAOHTTP_EXPORT char *getHttpFullDate(
            const xiaoLog::Date &date = xiaoLog::Date::now());

        /**
         * @brief Get a formatted string
         *
         * @param format
         * @param ...
         * @return XIAOHTTP_EXPORT
         */
        XIAOHTTP_EXPORT std::string formattedString(const char *format, ...);
    }
}