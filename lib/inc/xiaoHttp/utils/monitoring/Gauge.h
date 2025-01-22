/**
 * @file Gauge.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once
#include <xiaoHttp/utils/monitoring/Mertric.h>
#include <string_view>
#include <atomic>

namespace xiaoHttp
{
    namespace monitoring
    {
        /**
         * @brief This class is used to collect samples for a gauge metric.
         *
         */
        class Gauge : public Metric
        {
        public:
            Gauge(const std::string &name,
                  const std::vector<std::string> &lableNames,
                  const std::vector<std::string> &labelValues) noexcept(false)
                : Metric(name, lableNames, labelValues)
            {
            }

            std::vector<Sample> collect() const override
            {
                Sample s;
                std::lock_guard<std::mutex> lock(mutex_);
                s.name = name_;
                s.value = value_;
                s.timestamp = timestamp_;
                return {s};
            }

            /**
             * Increment the counter by 1.
             * */
            void increment()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                value_ += 1;
            }

            void decrement()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                value_ -= 1;
            }

            void decrement(double value)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                value_ -= value;
            }

            /**
             * Increment the counter by the given value.
             * */
            void increment(double value)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                value_ += value;
            }

            void reset()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                value_ = 0;
            }

            void set(double value)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                value_ = value;
            }

            static std::string_view type()
            {
                return "counter";
            }

            void setToCurrentTime()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                timestamp_ = xiaoLog::Date::now();
            }

        private:
            mutable std::mutex mutex_;
            double value_{0};
            xiaoLog::Date timestamp_{0};
        };
    }
}