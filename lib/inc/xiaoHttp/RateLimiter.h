/**
 * @file RateLimiter.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-21
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <memory>
#include <chrono>
#include <mutex>
#include <string>

namespace xiaoHttp
{
    enum class XIAOHTTP_EXPORT RateLimiterType
    {
        kFixedWindow,
        kSlidingWindow,
        kTokenBucket
    };

    inline RateLimiterType stringToRateLimiterType(const std::string &type)
    {
        if (type == "fixedWindow" || type == "fixed_window")
            return RateLimiterType::kFixedWindow;
        else if (type == "slidingWindow" || type == "sliding_window")
            return RateLimiterType::kSlidingWindow;
        return RateLimiterType::kTokenBucket;
    }
    class XIAOHTTP_EXPORT RateLimiter;
    using RateLimiterPtr = std::shared_ptr<RateLimiter>;

    /**
     * @brief This class is used to limit the number of requests per second
     *
     */
    class XIAOHTTP_EXPORT RateLimiter
    {
    public:
        /**
         * @brief Create a rate limiter
         *
         * @param type The type of the rate limiter
         * @param capacity The maximum number of requests in the time unit.
         * @param timeUnit  The time unit of the rate limiter.
         * @return RateLimiterPtr
         */
        static RateLimiterPtr newRateLimiter(
            RateLimiterType type,
            size_t capacity,
            std::chrono::duration<double> timeUnit = std::chrono::seconds(60));

        /**
         * @brief Check if a request is allowed
         *
         * @return true
         * @return false
         */
        virtual bool isAllowed() = 0;
        virtual ~RateLimiter() noexcept = default;
    };

    class XIAOHTTP_EXPORT SafeRateLimiter : public RateLimiter
    {
    public:
        SafeRateLimiter(RateLimiterPtr limiter) : limiter_(limiter)
        {
        }

        bool isAllowed() override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return limiter_->isAllowed();
        }

        ~SafeRateLimiter() noexcept override = default;

    private:
        RateLimiterPtr limiter_;
        std::mutex mutex_;
    }
}