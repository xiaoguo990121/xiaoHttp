/**
 * @file StopWatch.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once

#include <chrono>
#include <functional>
#include <assert.h>

namespace xiaoHttp
{
    /**
     * @brief This class is used to measure the elapsed time.
     *
     */
    class StopWatch
    {
    public:
        StopWatch() : strat_(std::chrono::steady_clock::now())
        {
        }

        ~StopWatch()
        {
        }

        void reset()
        {
            start_ = std::chrono::steady_clock::now();
        }

        double elapsed() const
        {
            return std::chrono::duration_cast<std::chrono::duration<double>>(
                       std::chrono::steady_clock::now() - start_)
                .count();
        }

    private:
        std::chrono::steady_clock::time_point start_;
    };

    class LifeTimeWatch
    {
    public:
        LifeTimeWatch(std::function<void(double)> callback)
            : stopWatch_(), callback_(std::move(callback))
        {
            assert(callback_);
        }

        ~LifeTimeWatch()
        {
            callback_(stopWatch_.elapsed());
        }

    private:
        StopWatch stopWatch_;
        std::function<void(double)> callback_;
    };
}