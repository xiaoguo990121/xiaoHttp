/**
 * @file Hodor.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#pragma once

#include <xiaoHttp/RateLimiter.h>
#include <xiaoHttp/plugins/Plugin.h>
#include <xiaoHttp/plugins/RealIpResolver.h>
#include <xiaoHttp/HttpAppFramework.h>
#include <xiaoHttp/CacheMap.h>
#include <regex>
#include <optional>

namespace xiaoHttp
{
    namespace plugin
    {
        class XIAOHTTP_EXPORT Hodor : public xiaoHttp::Plugin<Hodor>
        {
        public:
        private:
            struct LimitStrategy
            {
                std::regex urlsReges;
                size_t capacity{0};
                size_t ipCapacity{0};
                size_t userCapacity{0};
                bool regexFlag{false};
                RateLimiterPtr globalLimiterPtr;
                std::unique_ptr<CacheMap<std::string, RateLimiterPtr>> ipLimiterMapPtr;
                std::unique_ptr<CacheMap<std::string, RateLimiterPtr>>
                    userLimiterMapPtr;
            };

            LimitStrategy makeLimitStrategy(const Json::Value &config);
            std::vector<LimitStrategy> limitStrategies_;
            RateLimiterType algorithm_{RateLimiterType::kTokenBucket};
            std::chrono::duration<double> timeUnit_{1.0};
            bool multiThreads_{true};
            bool useRealIpResolver_{false};
            size_t limiterExpireTime_{600};
            std::function<std::optional<std::string>(const xiaoHttp::HttpRequestPtr &)>
                useIdGetter_;
            std::function<HttpResponsePtr(const xiaoHttp::HttpRequestPtr &)>
                rejectResponseFactory_;

            void onHttpRequest(const xiaoHttp::HttpRequestPtr &,
                               AdviceCallback &&,
                               AdviceChainCallback &&);

            bool checkLimit(const xiaoHttp::HttpRequestPtr &req,
                            const LimitStrategy &strategy,
                            const std::string &ip,
                            const std::optional<std::string> &userId);

            HttpResponsePtr rejectResponse_;
        };
    }
}