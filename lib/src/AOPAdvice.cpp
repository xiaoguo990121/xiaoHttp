/**
 * @file AOPAdvice.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#include "AOPAdvice.h"
#include <xiaoNet/net/TcpConnection.h>

namespace xiaoHttp
{
    static void doAdvicesChain(
        const std::vector<std::function<void(const HttpRequestPtr &,
                                             AdviceCallback &&,
                                             AdviceChainCallback &&)>> &advices,
        size_t index,
        const HttpRequestImplPtr &req,
        std::shared_ptr<const std::function<void(const HttpResponsePtr &)>>
            &&callbackPtr);

    bool AopAdvice::passNewConnectionAdvices(
        const xiaoNet::TcpConnectionPtr &conn) const
    {
        for (auto &advice : newConnectionAdvices_)
        {
            if (!advice(conn->localAddr(), conn->peerAddr()))
            {
                return false;
            }
        }
        return true;
    }

    void AopAdvice::passResponseCreationAdvices(const HttpResponsePtr &resp) const
    {
        if (!responseCreationAdvices_.empty())
        {
            for (auto &advice : responseCreationAdvices_)
            {
                advice(resp);
            }
        }
    }

    HttpResponsePtr AopAdvice::passSyncAdvices(const HttpRequestPtr &req) const
    {
        for (auto &advice : syncAdvices_)
        {
            if (auto resp = advice(req))
            {
                return resp;
            }
        }
        return nullptr;
    }

    void AopAdvice::passPreRoutingObservers(const HttpRequestImplPtr &req) const
    {
        if (!preRoutingObservers_.empty())
        {
            for (auto &observer : preRoutingObservers_)
            {
                observer(req);
            }
        }
    }

    void AopAdvice::passPreRoutingAdvices(
        const HttpRequestImplPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback) const
    {
        if (preRoutingAdvices_.empty())
        {
            callback(nullptr);
            return;
        }

        auto callbackPtr =
            std::make_shared<std::decay_t<decltype(callback)>>(std::move(callback));
        doAdvicesChain(preRoutingAdvices_, 0, req, std::move(callbackPtr));
    }

    void AopAdvice::passPostRoutingObservers(const HttpRequestImplPtr &req) const
    {
        if (!postRoutingObservers_.empty())
        {
            for (auto &observer : postRoutingObservers_)
            {
                observer(req);
            }
        }
    }

    void AopAdvice::passPostRoutingAdvices(
        const HttpRequestImplPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback) const
    {
        if (postRoutingAdvices_.empty())
        {
            callback(nullptr);
            return;
        }

        auto callbackPtr =
            std::make_shared<std::decay_t<decltype(callback)>>(std::move(callback));
        doAdvicesChain(postRoutingAdvices_, 0, req, std::move(callbackPtr));
    }

    void AopAdvice::passPreHandlingObservers(const HttpRequestImplPtr &req) const
    {
        if (!preHandlingObservers_.empty())
        {
            for (auto &observer : preHandlingObservers_)
            {
                observer(req);
            }
        }
    }

    void AopAdvice::passPreHandlingAdvices(
        const HttpRequestImplPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback) const
    {
        if (preHandlingAdvices_.empty())
        {
            callback(nullptr);
            return;
        }

        auto callbackPtr =
            std::make_shared<std::decay_t<decltype(callback)>>(std::move(callback));
        doAdvicesChain(preHandlingAdvices_, 0, req, std::move(callbackPtr));
    }

    void AopAdvice::passPostHandlingAdvices(const HttpRequestImplPtr &req,
                                            const HttpResponsePtr &resp) const
    {
        for (auto &advice : postHandlingAdvices_)
        {
            advice(req, resp);
        }
    }

    void AopAdvice::passPreSendingAdvices(const HttpRequestImplPtr &req,
                                          const HttpResponsePtr &resp) const
    {
        for (auto &advice : preSendingAdvices_)
        {
            advice(req, resp);
        }
    }

    static void doAdvicesChain(
        const std::vector<std::function<void(const HttpRequestPtr &,
                                             AdviceCallback &&,
                                             AdviceChainCallback &&)>> &advices,
        size_t index,
        const HttpRequestImplPtr &req,
        std::shared_ptr<const std::function<void(const HttpResponsePtr &)>>
            &&callbackPtr)
    {
        if (index < advices.size())
        {
            auto &advice = advices[index];
            advice(
                req,
                [callbackPtr](const HttpResponsePtr &resp)
                {
                    (*callbackPtr)(resp);
                },
                [index, req, callbackPtr, &adcices]() mutable
                {
                    auto ioLoop = req->getLoop();
                    if (ioLoop && !ioLoop->isInLoopThread())
                    {
                        ioLoop->queueInLoop([index,
                                             req,
                                             callbackPtr = std::move(callbackPtr),
                                             &advices]() mutable
                                            { doAdvicesChain(advices,
                                                             index + 1,
                                                             req,
                                                             std::move(callbackPtr)); });
                    }
                    else
                    {
                        doAdvicesChain(advices,
                                       index + 1,
                                       req,
                                       std::move(callbackPtr));
                    }
                });
        }
        else
        {
            (*callbackPtr)(nullptr);
        }
    }

}