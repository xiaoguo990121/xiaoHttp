/**
 * @file Cookie.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-15
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <xiaoLog/Date.h>
#include <xiaoLog/Logger.h>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

namespace xiaoHttp
{
    /**
     * @brief this class represents a cookie entity.
     *
     */
    class XIAOHTTP_EXPORT Cookie
    {
    public:
        Cookie(const std::string &key, const std::string &value)
            : key_(key), value_(value)
        {
        }

        Cookie(std::string &&key, std::string &&value)
            : key_(std::move(key)), value_(std::move(value))
        {
        }

        Cookie() = default;
        enum class SameSite
        {
            kNull,
            kLax,
            kStrict,
            kNone
        };

        /**
         * @brief Set the Expires Date object
         *
         * @param date
         */
        void setExpiresDate(const xiaoLog::Date &date)
        {
            expiresDate_ = date;
        }

        /**
         * @brief Set if the cookie is Http only.
         *
         * @param only
         */
        void setHttpOnly(bool only)
        {
            httpOnly_ = only;
        }

        /**
         * @brief Set if the cookie is secure.
         *
         * @param secure
         */
        void setSecure(bool secure)
        {
            secure_ = secure;
        }

        /**
         * @brief Set the Domain of the cookie.
         *
         * @param domain
         */
        void setDomain(const std::string &domain)
        {
            domain_ = domain;
        }

        void setDomain(std::string &&domain)
        {
            domain_ = std::move(domain);
        }

        /**
         * @brief Set the Path of the cookie.
         *
         * @param path
         */
        void setPath(const std::string &path)
        {
            path_ = path;
        }

        void setPath(std::string &&path)
        {
            path_ = std::move(path);
        }

        /**
         * @brief Set the key of the cookie.
         */
        void setKey(const std::string &key)
        {
            key_ = key;
        }

        void setKey(std::string &&key)
        {
            key_ = std::move(key);
        }

        /**
         * @brief Set the value of the cookie.
         */
        void setValue(const std::string &value)
        {
            value_ = value;
        }

        void setValue(std::string &&value)
        {
            value_ = std::move(value);
        }

        /**
         * @brief Set the max-age of the cookie.
         */
        void setMaxAge(int value)
        {
            maxAge_ = value;
        }

        /**
         * @brief Set the same site of the cookie.
         */
        void setSameSite(SameSite sameSite)
        {
            sameSite_ = sameSite;
        }

        /**
         * @brief Get the string value of the cookie
         *
         * @return std::string
         */
        std::string cookieString() const;

        /**
         * @brief Get the string value of the cookie
         */
        std::string getCookieString() const
        {
            return cookieString();
        }

        /**
         * @brief Get the expiration date of the cookie
         */
        const xiaoLog::Date &expiresDate() const
        {
            return expiresDate_;
        }

        /**
         * @brief Get the expiration date of the cookie
         */
        const xiaoLog::Date &getExpiresDate() const
        {
            return expiresDate_;
        }

        /**
         * @brief Get the domain of the cookie
         */
        const std::string &domain() const
        {
            return domain_;
        }

        /**
         * @brief Get the domain of the cookie
         */
        const std::string &getDomain() const
        {
            return domain_;
        }

        /**
         * @brief Get the path of the cookie
         */
        const std::string &path() const
        {
            return path_;
        }

        /**
         * @brief Get the path of the cookie
         */
        const std::string &getPath() const
        {
            return path_;
        }

        /**
         * @brief Get the keyword of the cookie
         */
        const std::string &key() const
        {
            return key_;
        }

        /**
         * @brief Get the keyword of the cookie
         */
        const std::string &getKey() const
        {
            return key_;
        }

        /**
         * @brief Get the value of the cookie
         */
        const std::string &value() const
        {
            return value_;
        }

        /**
         * @brief Get the value of the cookie
         */
        const std::string &getValue() const
        {
            return value_;
        }

        /**
         * @brief Check if the cookie is empty
         *
         * @return true
         * @return false
         */
        operator bool() const
        {
            return (!key_.empty()) && (!value_.empty());
        }

        /**
         * @brief Check if the cookie is HTTP only
         *
         * @return true means the cookie is HTTP only
         * @return false means the cookie is not HTTP only
         */
        bool isHttpOnly() const
        {
            return httpOnly_;
        }

        /**
         * @brief Check if the cookie is secure.
         *
         * @return true means the cookie is secure.
         * @return false means the cookie is not secure.
         */
        bool isSecure() const
        {
            return secure_;
        }

        /**
         * @brief Get the max-age of the cookie
         */
        std::optional<int> maxAge() const
        {
            return maxAge_;
        }

        /**
         * @brief Get the max-age of the cookie
         */
        std::optional<int> getMaxAge() const
        {
            return maxAge_;
        }

        /**
         * @brief Get the same site of the cookie
         */
        SameSite sameSite() const
        {
            return sameSite_;
        }

        /**
         * @brief Get the same site of the cookie
         */
        SameSite getSameSite() const
        {
            return sameSite_;
        }

        /**
         * @brief Compare two strings ignoring the their cases
         *
         * @param str1 string to check its value
         * @param str2 string to check against, written in lower case.
         * @return true
         * @return false
         *
         * @note the function is optimized to check for cookie's samesite value
         * where we check if the value equals to a specific value we already know in
         * str2. so the function doesn't apply tolower to the second argument
         * str2 as it's always in lower case.
         */
        static bool stricmp(const std::string_view str1,
                            const std::string_view str2)
        {
            auto str1Len{str1.length()};
            auto str2Len{str2.length()};
            if (str1Len != str2Len)
                return false;
            for (size_t idx{0}; idx < str1Len; ++idx)
            {
                auto lowerChar{tolower(str1[idx])};

                if (lowerChar != str2[idx])
                {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Converts a string value to its associated enum class SameSite
         * value
         *
         * @param sameSite
         * @return SameSite
         */
        static SameSite convertString2SameSite(const std::string_view &sameSite)
        {
            if (stricmp(sameSite, "lax"))
            {
                return Cookie::SameSite::kLax;
            }
            else if (stricmp(sameSite, "strict"))
            {
                return Cookie::SameSite::kStrict;
            }
            else if (stricmp(sameSite, "none"))
            {
                return Cookie::SameSite::kNone;
            }
            else if (!stricmp(sameSite, "null"))
            {
                LOG_WARN
                    << "'" << sameSite
                    << "' is not a valid SameSite policy. 'Null', 'Lax', 'Strict' "
                       "or "
                       "'None' are proper values. Return value is SameSite::kNull.";
            }
            return Cookie::SameSite::kNull;
        }

        /**
         * @brief Converts an enum class SameSite value to its associated string
         * value
         *
         * @param sameSite
         * @return const std::string_view&
         */
        static const std::string_view &convertSameSite2String(SameSite sameSite)
        {
            switch (sameSite)
            {
            case SameSite::kLax:
            {
                static std::string_view sv{"Lax"};
                return sv;
            }
            case SameSite::kStrict:
            {
                static std::string_view sv{"Strict"};
                return sv;
            }
            case SameSite::kNone:
            {
                static std::string_view sv{"None"};
                return sv;
            }
            case SameSite::kNull:
            {
                static std::string_view sv{"Null"};
                return sv;
            }
            }
            {
                static std::string_view sv{"UNDEFINED"};
                return sv;
            }
        }

    private:
        xiaoLog::Date expiresDate_{(std::numeric_limits<int64_t>::max)()};
        bool httpOnly_{true};
        bool secure_{false};
        std::string domain_;
        std::string path_;
        std::string key_;
        std::string value_;
        std::optional<int> maxAge_;
        SameSite sameSite_{SameSite::kNull};
    };
}