/**
 * @file Histogram.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once
#include <xiaoHttp/exports.h>
#include <xiaoHttp/utils/monitoring/Mertric.h>
#include <xiaoNet/net/EventLoopThread.h>
#include <string_view>
#include <atomic>
#include <mutex>

namespace xiaoHttp
{
    namespace monitoring
    {
        /**
         * @brief This class is used to collect samples for a counter metric.
         *
         */
        class XIAOHTTP_EXPORT Histogram : public Metric
        {
        public:
            struct TimeBucket
            {
                std::vector<uint64_t> buckets;
                uint64_t count{0};
                double sum{0};
            };

            Histogram(const std::string &name,
                      const std::vector<std::string> &labelNames,
                      const std::vector<std::string> &lableValues,
                      const std::vector<double> &bucketBoundaries,
                      const std::chrono::duration<double> &maxAge,
                      uint64_t timeBucketsCount,
                      xiaoNet::EventLoop *loop = nullptr) noexcept(false)
                : Metric(name, labelNames, lableValues),
                  maxAge_(maxAge),
                  timeBucketCount_(timeBucketsCount),
                  bucketBoundaries_(bucketBoundaries)
            {
                if (loop == nullptr)
                {
                    loopThreadPtr_ = std::make_unique<xiaoNet::EventLoopThread>();
                    loopPtr_ = loopThreadPtr_->getLoop();
                    loopThreadPtr_->run();
                }
                else
                {
                    loopPtr_ = loop;
                }
                if (maxAge > std::chrono::seconds(0))
                {
                    if (timeBucketsCount == 0)
                    {
                        throw std::runtime_error(
                            "timeBucketsCount must be greater than 0");
                    }
                }
                timeBuckets_.emplace_back();
                // check the bucket boundaries are sorted
                for (size_t i = 1; i < bucketBoundaries.size(); i++)
                {
                    if (bucketBoundaries[i] <= bucketBoundaries[i - 1])
                    {
                        throw std::runtime_error(
                            "The bucket boundaries must be sorted");
                    }
                }
            }

            void observe(double value);
            std::vector<Sample> collect() const override;

            ~Histogram() override
            {
                if (timerId_ != xiaoNet::InvalidTimerId)
                {
                    loopPtr_->invalidateTimer(timerId_);
                }
            }

            static std::string_view type()
            {
                return "histogram";
            }

        private:
            std::deque<TimeBucket> timeBuckets_;
            std::unique_ptr<xiaoNet::EventLoopThread> loopThreadPtr_;
            xiaoNet::EventLoop *loopPtr_{nullptr};
            mutable std::mutex mutex_;
            std::chrono::duration<double> maxAge_;
            xiaoNet::TimerId timerId_{xiaoNet::InvalidTimerId};
            size_t timeBucketCount_{0};
            const std::vector<double> bucketBoundaries_;

            void rotateTimeBuckets()
            {
                std::lock_guard<std::mutex> guard(mutex_);
                TimeBucket bucket;
                bucket.buckets.resize(bucketBoundaries_.size() + 1);
                timeBuckets_.emplace_back(std::move(bucket));
                if (timeBuckets_.size() > timeBucketCount_)
                {
                    auto expiredTimeBucket = timeBuckets_.front();
                    timeBuckets_.erase(timeBuckets_.begin());
                }
            }
        };
    }
}
