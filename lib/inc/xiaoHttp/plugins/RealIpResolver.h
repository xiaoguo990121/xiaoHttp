/**
 * @file RealIpResolver.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoHttp/plugins/Plugin.h>
#include <xiaoNet/net/InetAddress.h>
#include <xiaoHttp/HttpRequest.h>
#include <vector>
#include <string>

namespace xiaoHttp
{
    namespace plugin
    {
        /**
        * @brief This plugin is used to resolve client real ip from HTTP request.
        * @note This plugin currently supports only ipv4 address or cidr.
        *
        * The json configuration is as follows:
        *
        * @code
          {
             "name": "drogon::plugin::RealIpResolver",
             "dependencies": [],
             "config": {
                // Trusted proxy ip or cidr
                "trust_ips": ["127.0.0.1", "172.16.0.0/12"],
                // Which header to parse ip form. Default is x-forwarded-for
                "from_header": "x-forwarded-for",
                // The result will be inserted to HttpRequest attribute map with this
                // key. Default is "real-ip"
                "attribute_key": "real-ip"
             }
          }
          @endcode
        *
        * Enable the plugin by adding the configuration to the list of plugins in the
        * configuration file.
        *
        */
        class XIAOHTTP_EXPORT RealIpResolver : public xiaoHttp::Plugin<RealIpResolver>
        {
        public:
            RealIpResolver()
            {
            }

            void initAndStart(const Json::Value &config) override;
            void shutdown() override;

            static const xiaoNet::InetAddress &GetRealAddr(
                const xiaoHttp::HttpRequestPtr &req);

        private:
            const xiaoNet::InetAddress &getRealAddr(
                const xiaoHttp::HttpRequestPtr &req) const;
            bool matchCidr(const xiaoNet::InetAddress &addr) const;

            struct CIDR
            {
                explicit CIDR(const std::string &ipOrCidr);
                in_addr_t addr_{0};
                in_addr_t mask_{32};
            };

            std::vector<CIDR> trustCIDRs_;
            std::string fromHeader_;
            std::string attributeKey_;
            bool useXForwardedFor_{false};
        }

    }

}