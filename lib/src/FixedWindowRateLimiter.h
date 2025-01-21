/**
 * @file FixedWindowRateLimiter.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-21
 *
 *
 */

#pragma once

#include <xiaoHttp/RateLimiter.h>
#include <chrono>

namespace xiaoHttp
{
    class FixedWindowRateLimiter : public RateLimiter
    {
    public:
        FixedWindowRateLimiter(size_t capacity,
                               std::chrono::duration<double> timeUnit);

        bool isAllowed() override;
        ~FixedWindowRateLimiter() noexcept override = default;

    private:
        size_t capacity_;
        size_t currentRequests_{0};
        std::chrono::steady_clock::time_point lastTime_;
        std::chrono::duration<double> timeUnit_;
    };
}
