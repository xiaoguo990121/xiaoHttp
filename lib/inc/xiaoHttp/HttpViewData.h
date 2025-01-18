/**
 * @file HttpViewData.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoLog/Logger.h>

#include <xiaoHttp/exports.h>

#include <unordered_map>
#include <any>
#include <string>
#include <stdarg.h>

namespace xiaoHttp
{
    /**
     * @brief This class represents the data set displayed in views.
     *
     */
    class XIAOHTTP_EXPORT HttpViewData
    {
    public:
        /**
         * @brief The function template is used to get an item in the data set by the key
         * parameter.
         *
         * @tparam T
         * @param key
         * @return const T&
         */
        template <typename T>
        const T &get(const std::string &key) const
        {
            static const T nullVal = T();
            auto it = viewData_.find(key);
            if (it != viewData_.end())
            {
                if (typeid(T) == it->second.type())
                {
                    return *(std::any_cast<T>(&(it->second)));
                }
                else
                {
                    LOG_ERROR << "Bad type";
                }
            }
            return nullVal;
        }

        /// Insert an item identified by the key parameter into the data set;
        void insert(const std::string &key, std::any &&obj)
        {
            viewData_[key] = std::move(obj);
        }

        void insert(const std::string &key, const std::any &obj)
        {
            viewData_[key] = obj;
        }

        /// Insert an item identified by the key parameter into the data set; The
        /// item is converted to a string.
        template <typename T>
        void insertAsString(const std::string &key, T &&val)
        {
            std::stringstream ss;
            ss << val;
            viewData_[key] = ss.str();
        }

        /// Insert a formatted string identified by the key parameter.
        void insertFormattedString(const std::string &key, const char *format, ...)
        {
            std::string strBuffer;
            strBuffer.resize(128);
            va_list ap, backup_ap;
            va_start(ap, format);
            va_copy(backup_ap, ap);
            auto result = vsnprintf((char *)strBuffer.data(),
                                    strBuffer.size(),
                                    format,
                                    backup_ap);
            va_end(backup_ap);
            if ((result >= 0) &&
                ((std::string::size_type)result < strBuffer.size()))
            {
                strBuffer.resize(result);
            }
            else
            {
                while (true)
                {
                    if (result < 0)
                    {
                        // Older snprintf() behavior. Just try doubling the buffer
                        // size
                        strBuffer.resize(strBuffer.size() * 2);
                    }
                    else
                    {
                        strBuffer.resize(result + 1);
                    }

                    va_copy(backup_ap, ap);
                    result = vsnprintf((char *)strBuffer.data(),
                                       strBuffer.size(),
                                       format,
                                       backup_ap);
                    va_end(backup_ap);

                    if ((result >= 0) &&
                        ((std::string::size_type)result < strBuffer.size()))
                    {
                        strBuffer.resize(result);
                        break;
                    }
                }
            }
            va_end(ap);
            viewData_[key] = std::move(strBuffer);
        }

        /// Get the 'any' object by the key parameter.
        std::any &operator[](const std::string &key) const
        {
            return viewData_[key];
        }

        /// Translate some special characters to HTML format
        /**
         * such as:
         * @code
           " --> &quot;
           & --> &amp;
           < --> &lt;
           > --> &gt;
           @endcode
         */
        static std::string htmlTranslate(const char *str, size_t length);

        static std::string htmlTranslate(const std::string_view &str)
        {
            return htmlTranslate(str.data(), str.length());
        }

        static bool needTranslation(const std::string_view &str)
        {
            for (auto const &c : str)
            {
                switch (c)
                {
                case '"':
                case '&':
                case '<':
                case '>':
                    return true;
                default:
                    continue;
                }
            }
            return false;
        }

    protected:
        using ViewDataMap = std::unordered_map<std::string, std::any>;
        mutable ViewDataMap viewData_;
    };

}