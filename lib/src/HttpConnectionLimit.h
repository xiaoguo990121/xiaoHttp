/**
 * @file HttpConnectionLimit.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <xiaoNet/net/TcpConnection.h>

namespace xiaoHttp
{
    class HttpConnectionLimit
    {
    public:
        static HttpConnectionLimit &instance()
        {
            static HttpConnectionLimit inst;
            return inst;
        }

        size_t getConnectionNum() const
        {
            return connectionNum_.load(std::memory_order_relaxed);
        }

        // don't set after start
        void setMaxConnectionNum(size_t num);
        void setMaxConnectionNumPerIP(size_t num);

        bool tryAddConnection(const xiaoNet::TcpConnectionPtr &conn);
        void releaseConnection(const xiaoNet::TcpConnectionPtr &conn);

    private:
        std::mutex mutex_;

        size_t maxConnectionNum_{100000};
        std::atomic<size_t> connectionNum_{0};

        size_t maxConnectionNumPerIP_{0};
        std::unordered_map<std::string, size_t> ipConnectionsMap_;
    };
}