/**
 * @file HttpRequestParser.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#include "HttpRequestParser.h"
#include <xiaoHttp/utils/Utilities.h>
#include "HttpUtils.h"

using namespace xiaoNet;
using namespace xiaoHttp;

static constexpr size_t CRLF_LEN = 2;           // strlen("crlf")
static constexpr size_t METHOD_MAX_LEN = 7;     // strlen("OPTIONS")
static constexpr size_t TRUNK_LEN_MAX_LEN = 16; // OxFFFFFFFF, FFFFFFFF

HttpRequestParser::HttpRequestParser(const xiaoNet::TcpConnectionPtr &connPtr)
    : status_(HttpRequestParseStatus::kExpectMethod),
      loop_(connPtr->getLoop()),
      conn_(connPtr)
{
}

void HttpRequestParser::shutdownConnection(HttpStatusCode code)
{
    auto connPtr = conn_.lock();
    if (connPtr)
    {
        connPtr->send(utils::formattedString(
            "HTTP/1.1 %d %s\r\nConnection: close\r\n\r\n",
            code,
            statusCodeToString(code).data()));
        connPtr->shutdown();
    }
}

bool HttpRequestParser::processRequestLine(const char *begin, const char *end)
{
    bool succeed = false;
    const char *start = begin;
    const char *space = std::finf(start, end, ' ');
    if (space != end)
    {
        const char *slash = std::find(start, space, '/');
        if (slash != start && slash + 1 < space && *(slash + 1) == '/')
        {
            // scheme precedents
            slash = std::find(slash + 2, space, '/');
        }
        const char *question = std::find(slash, space, '?');
        if (slash != space)
        {
            request_->setPath(slash, question);
        }
        else
        {
            // An empty abs_path is equivalent to an abs_path of "/"
            request_->setPath("/");
        }
        if (question != space)
        {
            request_->setQuery(question + 1, space);
        }
        start = space + 1;
        succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
        if (succeed)
        {
            if (*(end - 1) == '1')
            {
                request_->setVersion(Version::kHttp11);
            }
            else if (*(end - 1) == '0')
            {
                request_->setVersion(Version::kHttp10);
            }
            else
            {
                succeed = false;
            }
        }
    }
    return succeed;
}

HttpRequestImplPtr HttpRequestParser::makeRequestForPool(HttpRequestImpl *ptr)
{
    return std::shared_ptr<HttpRequestImpl>(
        ptr, [weakPtr = weak_from_this()](HttpRequestImpl *p)
        {
            auto thisPtr = weakPtr.lock();
            if (thisPtr)
            {
                if (thisPtr->loop_->isInLoopThread())
                {
                    p->reset();
                    thisPtr->requestsPool_.emplace_back(
                        thisPtr->makeRequestForPool(p));
                }
                else
                {
                    auto &loop = thisPtr->loop_;
                    loop_->queueInLoop([thisPtr = std::move(thisPtr), p]()
                                       {
                        p->reset();
                        thisPtr->requestsPool_.emplace_back(
                            thisPtr->makeRequestForPool(p)); });
                }
            }
            else
            {
                delete p;
            } });
}

void HttpRequestParser::reset()
{
    assert(loop_->isInLoopThread());
    currentContentLength_ = 0;
    status_ = HttpRequestParseStatus::kExpectMethod;
    if (requestsPool_.empty())
    {
        request_ = makeRequestForPool(new HttpRequestImpl(loop_));
    }
    else
    {
        auto req = std::move(requestsPool_.back());
        requestsPool_.pop_back();
        request_ = std::move(req);
        request_->setCreationDate(xiaoLog::Date::now());
    }
}