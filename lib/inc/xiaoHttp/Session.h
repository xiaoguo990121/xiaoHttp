/**
 * @file Session.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-15
 *
 *
 */

#pragma once

#include <xiaoLog/Logger.h>
#include <any>
#include <map>
#include <optional>

namespace xiaoHttp
{
    /**
     * @brief This class represents a session stored in the framework.
     * One can get or set any type of data to a session object.
     *
     */
    class Session
    {
    public:
        using SessionMap = std::map<std::string, std::any>;

        /**
         * @brief Get the data identified by the key parameter.
         * @note if the data is not found, a default value is returned.
         * For example:
         * @code
         * auto userName = sessionPtr->get<std::string>("user name");
         * @tparam T
         * @param key
         * @return T
         */
        template <typename T>
        T get(const std::string &key) const
        {
            {
                std::lock_guard<std::mutex> lck(mutex_);
                auto it = sessionMap_.find(key);
                if (it != sessionMap_.end())
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
            }
            return T();
        }

        /**
         * @brief Get the data identified by the key paramter and return an
         * optional object that wraps the data.
         *
         * @tparam T
         * @param key
         * @return std::optional<T>
         */
        template <typename T>
        std::optional<T> getOptional(const std::string &key) const
        {
            {
                std::lock_guard<std::mutex> lck(mutex_);
                auto it = sessionMap_.find(key);
                if (it != sessionMap_.end())
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
            }
            return std::nullopt;
        }

        /**
         * @brief Modify or visit the data identified by the key paramter.
         *
         * @tparam T the data type of the data.
         * @tparam Callable
         * @param key
         * @param handler A callable that can modify or visit the data. The
         * signature of the handler should be equivalent to 'void(T&)' or
         * 'void(const T&)'
         *
         * @note This function is multiple-thread safe. if the data identified by
         * the key doesn't exist, a new one is created and passed to the handler.
         * The changing of the data is protected by the mutex of the session.
         */
        template <typename T, typename Callable>
        void modify(const std::string &key, Callable &handler)
        {
            std::lock_guard<std::mutex> lck(mutex_);
            auto it = sessionMap_.find(key);
            if (it != sessionMap_.end())
            {
                if (typeid(T) == it->second.type())
                {
                    handler(*(std::any_cast<T>(&(it->second))));
                }
                else
                {
                    LOG_ERROR << "Bad type";
                }
            }
            else
            {
                auto item = T();
                handler(item);
                sessionMap_.insert(std::make_pair(key, std::any(std::move(item))));
            }
        }

        /**
         * @brief Modify or visit the session data.
         *
         * @tparam Callable: The signature of the callable should be equivalent to
         * 'void(Session::SessionMap &)' or 'void (const Session::SessionMap &)'
         * @param handler A callable that can modify the sessionMap_ inside the
         * session.
         */
        template <typename Callable>
        void modify(Callable &&handler)
        {
            std::lock_guard<std::mutex> lck(mutex_);
            handler(sessionMap_);
        }

        /**
         * @brief Insert a key-value pair
         *
         * @param key
         * @param obj
         */
        void insert(const std::string &key, const std::any &obj)
        {
            std::lock_guard<std::mutex> lck(mutex_);
            sessionMap_.insert(std::make_pair(key, obj));
        }

        void insert(const std::string &key, std::any &&obj)
        {
            std::lock_guard<std::mutex> lck(mutex_);
            sessionMap_.insert(std::make_pair(key, std::move(obj)));
        }

        /**
         * @brief Erase the data identified by the given key.
         *
         * @param key
         */
        void erase(const std::string &key)
        {
            std::lock_guard<std::mutex> lck(mutex_);
            sessionMap_.erase(key);
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
            std::lock_guard<std::mutex> lck(mutex_);
            if (sessionMap_.find(key) == sessionMap_.end())
            {
                return false;
            }
            return true;
        }

        /**
         * @brief Clear all data in the seesion.
         *
         */
        void clear()
        {
            std::lock_guard<std::mutex> lck(mutex_);
            sessionMap_.clear();
        }

        /**
         * @brief Get the session ID of the current session.
         *
         * @return std::string
         */
        std::string sessionId() const
        {
            std::lock_guard<std::mutex> lck(mutex_);
            return sessionId_;
        }

        /**
         * @brief Let the framework create a new seesion ID for this session and set
         * it to the client.
         * @note This method does not change the session ID now.
         */
        void changeSessionIdToClient()
        {
            needToChange_ = true;
            needToSet_ = true;
        }

        Session() = delete;

    private:
        SessionMap sessionMap_;
        mutable std::mutex mutex_;
        std::string sessionId_;
        bool needToSet_{false};
        bool needToChange_{false};
        friend class SessionManager;
        friend class HttpAppFrameworkImpl;

        /**
         * @brief Constructor, usually called by the framework
         *
         * @param id
         * @param needToSet
         */
        Session(const std::string &id, bool needToSet)
            : sessionId_(id), needToSet_(needToSet)
        {
        }

        /**
         * @brief Change the state of the session, usually called by the framework
         *
         */
        void hasSet()
        {
            needToSet_ = false;
        }

        /**
         * @brief If the session ID needs to be changed.
         *
         * @return true
         * @return false
         */
        bool needToChangeSessionId() const
        {
            return needToChange_;
        }

        /**
         * @brief If the session ID needs to be set to the client through cookie,
         * return true
         *
         * @return true
         * @return false
         */
        bool needSetToClient() const
        {
            return needToSet_;
        }

        void setSessionId(const std::string &id)
        {
            std::lock_guard<std::mutex> lck(mutex_);
            sessionId_ = id;
            needToChange_ = false;
        }
    };

    using SessionPtr = std::shared_ptr<Session>;
} // namespace xiaoHttp
