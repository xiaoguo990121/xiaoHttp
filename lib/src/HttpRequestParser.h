/**
 * @file HttpRequestParser.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoHttp/HttpTypes.h>
#include <xiaoNet/net/TcpConnection.h>
#include <xiaoNet/utils/MsgBuffer.h>
#include <xiaoNet/utils/NonCopyable.h>
#include <memory>
#include <deque>

#include "impl_forwards.h"

namespace xiaoHttp
{
    class HttpRequestParser : public xiaoNet::NonCopyable,
                              public std::enable_shared_from_this<HttpRequestParser>
    {
    public:
        enum class HttpRequestParseStatus
        {
            kExpectMethod,
            kExpectRequestLine,
            kExpectHeaders,
            kExpectBody,
            kExpectChunkLen,
            kExpectChunkBody,
            kExpectLastEmptyChunk,
            kGotAll,
        };

        explicit HttpRequestParser(const xiaoNet::TcpConnectionPtr &connPtr);

        int parseRequest(xiaoNet::MsgBuffer *buf);

        bool gotAll() const
        {
            return status_ == HttpRequestParseStatus::kGotAll;
        }

        void reset();

        const HttpRequestImplPtr &requestImpl() const
        {
            return request_;
        }

        bool firstReq()
        {
            if (firstRequest_)
            {
                firstRequest_ = false;
                return true;
            }
            return false;
        }

        const WebSocketConnectionImplPtr &webSocketConn() const
        {
            return websockConnPtr_;
        }

        void setWebsockConnection(const WebSocketConnectionImplPtr &conn)
        {
            websockConnPtr_ = conn;
        }

        // to support request pipelining
        void pushRequestToPipelining(const HttpRequestPtr &, bool isHeadMethod);
        bool pushResponseToPipelining(const HttpRequestPtr &, HttpResponsePtr);
        void popReadyResponse(std::vector<std::pair<HttpResponsePtr, bool>> &);

        size_t numberOfRequestsInPipelining() const
        {
            return requestPipelining_.size();
        }

        bool emptyPipelining()
        {
            return requestPipelining_.empty();
        }

        bool isStop() const
        {
            return stopWorking_;
        }

        void stop()
        {
            stopWorking_ = true;
        }

        size_t numberOfRequestsParsed() const
        {
            return requestsCounter_;
        }

        xiaoNet::MsgBuffer &getBuffer()
        {
            return sendBuffer_;
        }

        std::vector<std::pair<HttpResponsePtr, bool>> &getResponseBuffer()
        {
            assert(loop_->isInLoopThread());
            if (!responseBuffer_)
            {
                responseBuffer_ =
                    std::unique_ptr<std::vector<std::pair<HttpResponsePtr, bool>>>(
                        new std::vector<std::pair<HttpResponsePtr, bool>>);
            }
            return *responseBuffer_;
        }

        std::vector<HttpRequestImplPtr> &getRequestBuffer()
        {
            assert(loop_->isInLoopThread());
            if (!requestBuffer_)
            {
                requestBuffer_ = std::unique_ptr<std::vector<HttpRequestImplPtr>>(
                    new std::vector<HttpRequestImplPtr>);
            }
            return *requestBuffer_;
        }

    private:
        HttpRequestImplPtr makeRequestForPool(HttpRequestImpl *p);
        void shutdownConnection(HttpStatusCode code);
        bool processRequestLine(const char *begin, const char *end);
        HttpRequestParseStatus status_;
        xiaoNet::EventLoop *loop_;
        HttpRequestImplPtr request_;
        bool firstRequest_{true};
        WebSocketConnectionImplPtr websockConnPtr_;
        std::deque<std::pair<HttpRequestPtr, std::pair<HttpResponsePtr, bool>>>
            requestPipelining_;
        size_t requestsCounter_{0};
        std::weak_ptr<xiaoNet::TcpConnection> conn_;
        bool stopWorking_{false};
        xiaoNet::MsgBuffer sendBuffer_;
        std::unique_ptr<std::vector<std::pair<HttpResponsePtr, bool>>>
            responseBuffer_;
        std::unique_ptr<std::vector<HttpRequestImplPtr>> requestBuffer_;
        std::vector<HttpRequestImplPtr> requestsPool_;
        size_t currentChunkLength_{0};
        size_t currentContentLength_{0};
    };
}