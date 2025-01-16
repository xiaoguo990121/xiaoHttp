/**
 * @file Attribute.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoLog/Logger.h>
#include <map>
#include <memory>
#include <any>

namespace xiaoHttp
{
    /**
     * @brief This class represents the attributes stored in the Http request.
     * One can add/get any type of data to/from an Attributes object.
     *
     */
    class Attributes
    {
    public:
        /**
         * @brief Get the data identified by the key parameter.
         * @note if the data is not found, a default value is returned.
         * For example:
         * @code
         *   auto &userName = attributesPtr->get<std::string>("user name");
         * @endcode
         */
        template <typename T>
        const T &get(const std::string &key) const
        {
            static const T nullVal = T();
            auto it = attributesMap_.find(key);
            if (it != attributesMap_.end())
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

        /**
         * @brief Get the 'any' object identified by the given key
         *
         * @param key
         * @return std::any&
         */
        std::any &operator[](const std::string &key)
        {
            return attributesMap_[key];
        }

        /**
         * @brief Insert a key-value pair
         * @note here the any object can be created implicitly. for example
         * @code
         * attributesPtr->insert("user name", userNameString);
         * @endcode
         */
        void insert(const std::string &key, const std::any &obj)
        {
            attributesMap_[key] = obj;
        }

        /**
         * @brief Erase the data identified by the given key.
         *
         * @param key
         */
        void erase(const std::string &key)
        {
            attributesMap_.erase(key);
        }

        /**
         * @brief Return true if the data identified by the key exists.
         *
         * @param key
         * @return true
         * @return false
         */
        bool find(const std::string &key)
        {
            if (attributesMap_.find(key) == attributesMap_.end())
            {
                return false;
            }
            return true;
        }

        /**
         * @brief Clear all attributes.
         *
         */
        void clear()
        {
            attributesMap_.clear();
        }

        /**
         * @brief Construct a new Attributes object
         *
         */
        Attributes() = default;

    private:
        using AttributesMap = std::map<std::string, std::any>;
        AttributesMap attributesMap_;
    };

    using AttributesPtr = std::shared_ptr<Attributes>;
}