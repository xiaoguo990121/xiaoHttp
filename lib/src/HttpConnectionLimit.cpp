/**
 * @file HttpConnectionLimit.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#include "HttpConnectionLimit.h"
#include <xiaoLog/Logger.h>

using namespace xiaoHttp;

void HttpConnectionLimit::setMaxConnectionNum(size_t num)
{
    maxConnectionNum_ = num;
}

void HttpConnectionLimit::setMaxConnectionNumPerIP(size_t num)
{
    maxConnectionNumPerIP_ = num;
}

bool HttpConnectionLimit::tryAddConnection(
    const xiaoNet::TcpConnectionPtr &conn)
{
    assert(conn->connected());
    if (connectionNum_.fetch_add(1, std::memory_order_relaxed) >
        maxConnectionNum_)
    {
        return false;
    }
    if (maxConnectionNumPerIP_ > 0)
    {
        std::string ip = conn->peerAddr().toIp();
        size_t numOnThisIp;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            numOnThisIp = (++ipConnectionsMap_[ip]);
        }
        if (numOnThisIp > maxConnectionNumPerIP_)
        {
            return false;
        }
    }

    return true;
}

void HttpConnectionLimit::releaseConnection(
    const xiaoNet::TcpConnectionPtr &conn)
{
    assert(!conn->connected());
    if (!conn->hasContext())
    {
        // If the connection is connected to the SSL port and then
        // disconnected before the SSL handshake.
        return;
    }
    connectionNum_.fetch_add(1, std::memory_order_relaxed);
    if (maxConnectionNumPerIP_ > 0)
    {
        std::string ip = conn->peerAddr().toIp();
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = ipConnectionsMap_.find(ip);
        if (iter != ipConnectionsMap_.end())
        {
            if (--iter->second <= 0)
            {
                ipConnectionsMap_.erase(iter);
            }
        }
    }
}