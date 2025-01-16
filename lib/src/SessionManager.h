/**
 * @file SessionManager.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoHttp/Session.h>
#include <xiaoHttp/CacheMap.h>
#include <xiaoNet/utils/NonCopyable.h>
#include <xiaoNet/net/EventLoop.h>
#include <xiaoHttp/xiaoHttp_callbacks.h>

namespace xiaoHttp
{
    class SessionManager : public xiaoNet::NonCopyable
    {
    public:
        using IdGeneratorCallback = std::function<std::string()>;

        SessionManager(
            xiaoNet::EventLoop *loop,
            size_t timeout,
            const std::vector<AdviceStartSessionCallback> &startAdvices,
            const std::vector<AdviceDestorySessionCallback> &destoryAdvices,
            IdGeneratorCallback idGeneratorCallback);

        ~SessionManager()
        {
            sessionMapPtr_.reset();
        }

        SessionPtr getSession(const std::string &sessionID, bool needToSet);
        void changeSessionId(const SessionPtr &sessionPtr);

    private:
        std::unique_ptr<CacheMap<std::string, SessionPtr>> sessionMapPtr_;
        xiaoNet::EventLoop *loop_;
        size_t timeout_;
        const std::vector<AdviceStartSessionCallback> &sessionStartAdvices_;
        const std::vector<AdviceDestorySessionCallback> &sessionDestroyAdvices_;
        IdGeneratorCallback idGeneratorCallback_;
    };
}