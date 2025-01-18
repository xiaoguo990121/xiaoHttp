/**
 * @file ListenerManager.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoNet/net/EventLoopThreadPool.h>
#include <xiaoNet/utils/NonCopyable.h>
#include <string>
#include <vector>
#include "impl_forwards.h"

namespace xiaoNet
{
    class InetAddress;
}

namespace xiaoHttp
{
    class ListenerManager : public xiaoNet::NonCopyable
    {
    public:
        ~ListenerManager() = default;
        void addListener(const std::string &ip,
                         uint16_t port,
                         bool useSSL = false,
                         const std::string &certFile = "",
                         const std::string &keyFile = "",
                         bool useOldTLS = false,
                         const std::vector<std::pair<std::string, std::string>>
                             &sslConfCmds = {});
        std::vector<xiaoNet::InetAddress> getListeners() const;
        void createListeners(
            const std::string &globalCertFile,
            const std::string &globalKeyFile,
            const std::vector<std::pair<std::string, std::string>> &sslConfCmds,
            const std::vector<xiaoNet::EventLoop *> &ipLoops);
        void startListening();
        void stopListening();

    private:
        struct ListenerInfo
        {
            ListenerInfo(
                std::string ip,
                uint16_t port,
                bool useSSL,
                std::string certFile,
                std::string keyFile,
                bool useOldTLS,
                std::vector<std::pair<std::string, std::string>> sslConfCmds)
                : ip_(std::move(ip)),
                  port_(port),
                  useSSL_(useSSL),
                  certFile_(std::move(certFile)),
                  keyFile_(std::move(keyFile)),
                  useOldTLS_(useOldTLS),
                  sslConfCmds_(std::move(sslConfCmds))
            {
            }

            std::string ip_;
            uint16_t port_;
            bool useSSL_;
            std::string certFile_;
            std::string keyFile_;
            bool useOldTLS_;
            std::vector<std::pair<std::string, std::string>> sslConfCmds_;
        };

        std::vector<ListenerInfo> listeners_;
        std::vector<std::shared_ptr<HttpServer>> servers_;

        std::unique_ptr<xiaoNet::EventLoopThread> listeningThread_;
    };
}