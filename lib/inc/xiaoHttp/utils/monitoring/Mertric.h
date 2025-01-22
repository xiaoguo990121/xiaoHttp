/**
 * @file Mertric.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once

#include <xiaoHttp/utils/monitoring/Sample.h>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace xiaoHttp
{
    namespace monitoring
    {
        /**
         * @brief This class is used to collect samples for a mertic.
         *
         */
        class Metric : public std::enable_shared_from_this<Metric>
        {
        public:
            Metric(const std::string &name,
                   const std::vector<std::string> &labelNames,
                   const std::vector<std::string> &labelValues) noexcept(false)
                : name_(name)
            {
                if (labelNames.size() != labelValues.size())
                {
                    throw std::runtime_error(
                        "The number of label names is not equal to the number of label values!");
                }
                labels_.resize(labelNames.size());
                for (size_t i = 0; i < labelNames.size(); ++i)
                {
                    labels_[i].first = labelNames[i];
                    labels_[i].second = labelValues[i];
                }
            };

            const std::string &name() const
            {
                return name_;
            }

            const std::vector<std::pair<std::string, std::string>> &labels() const
            {
                return labels_;
            }

            virtual ~Metric() = default;
            virtual std::vector<Sample> collect() const = 0;

        protected:
            const std::string name_;
            std::vector<std::pair<std::string, std::string>> labels_;
        };

        using MetricPtr = std::shared_ptr<Metric>;
    }
}