/**
 * @file Registry.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once
#include <memory>

namespace xiaoHttp
{
    namespace monitoring
    {
        class CollectorBase;

        /**
         * @brief This class is used to register metrics.
         *
         */
        class Registry
        {
        public:
            virtual ~Registry() = default;
            virtual void registerCollector(
                const std::shared_ptr<CollectorBase> &collector) = 0;
        };
    }
}