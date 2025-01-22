/**
 * @file Counter.h
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
#include <mutex>

namespace xiaoHttp
{
    namespace monitoring
    {
        class Counter : public Metric
        {
        public:
            Counter(const std::string &name,
                    cosnt std::vector<std::string> &labelNames,
                    const std::vector<std::string> &labelValues) noexcept(false)
                : Metric(name, labelNames, labelValues)
            {
            }

            std::vector<Sample> collect() const override
            {
                Sample s;
                s.name = name_;
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    s.value = value_;
                }
                return {s};
            }

            void increment()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                value_++;
            }

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

            static std::string_view type()
            {
                return "counter";
            }

        private:
            mutable std::mutex mutex_;
            double value_{0};
        };
    }
}