/**
 * @file HttpServer.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#include "HttpServer.h"

#include <xiaoLog/Logger.h>

#include "AOPAdvice.h"
#include "HttpAppFrameworkImpl.h"
#include "HttpConnectionLimit.h"

#if COZ_PROFILING
#include <coz.h>
#else
#define COZ_PROGRESS
#define COZ_PROGRESS_NAMED(name)
#define COZ_BEGIN(name)
#define COZ_END(name)
#endif

using namespace xiaoHttp;
using namespace xiaoNet;

static inline bool isWebSocket(const HttpRequestImplPtr &req);
static inline HttpResponsePtr tryDecompressRequest(
    const HttpRequestImplPtr &req);
static inline bool passSyncAdvices(
    const HttpRequestImplPtr &req,
    const std::shared_ptr<HttpRequestParser> &requestParser,
    bool shouldBePipelined,
    bool isHeadMethod);
static inline HttpResponsePtr getCompressdResponse(
    const HttpRequestImplPtr &req,
    const HttpResponsePtr &response,
    bool isHeadMethod);

static void handleInvalidHttpMethod(
    const HttpRequestImplPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback);

static void handleHttpOptions(
    const HttpRequestImplPtr &req,
    const std::string &allowMethods,
    std::function<void(const HttpResponsePtr &)> &&callback);

HttpServer::HttpServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       std::string name)
#ifdef __linux__
    : server_(loop, listenAddr, std::move(name))
#else
    : server_(loop, listenAddr, std::move(name), true, app().reusePort())
#endif
{
    server_.setConnectionCallback(onConnection);
    server_.setRecvMessageCallback(onMessage);
    server_.kickoffIdleConnections(
        HttpAppFrameworkImpl::instance().getIdleConnectionTimeout());
}

HttpServer::~HttpServer() = default;

void HttpServer::start()
{
    LOG_TRACE << "HttpServer[" << server_.name() << "] starts listening on "
              << server_.ipPort();
    server_.start();
}

void HttpServer::stop()
{
    server_.stop();
}

void HttpServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        auto parser = std::make_shared<HttpRequestParser>(conn);
        parser->reset();
        conn->setContext(parser);
        if (!HttpConnectionLimit::instance().tryAddConnection(conn))
        {
            LOG_ERROR << "too much connections!force close!";
            conn->forceClose();
            return;
        }
        if (!AopAdvice::instance().passNewConnectionAdvices(conn))
        {
            conn->foreceClose();
        }
    }
    else if (conn->disconnected())
    {
        LOG_TRACE << "conn disconnected!";
        HttpConnectionLimit::instance().releaseConnection(conn);
        auto requestParser = conn->getContext<HttpRequestParser>();
        if (requestParser)
        {
            if (requestParser->webSocketConn())
            {
                requestParser->webSocketConn()->onClose();
            }
            conn->clearContext();
        }
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn, MsgBuffer *buf)
{
    if (!conn->hasContext())
        return;
    auto requestParser = conn->getContext<HttpRequestParser>();
    if (!requestParser)
        return;
    if (requestParser->webSocketConn())
    {
        requestParser->webSocketConn()->onNewMessage(conn, buf);
        return;
    }

    auto &requests = requestParser->getRequestBuffer();
    // With the pipelining feature or web socket, it is possible to receive
    // multiple messages at once, so the while loop is necessary
    while (buf->readableBytes() > 0)
    {
        if (requestParser->isStop())
        {
            // The number of requests has reached the limit.
            buf->retrieveAll();
            return;
        }
        int parseRes = requestParser->parseRequest(buf);
        if (parseRes < 0)
        {
            requestParser->reset();
            conn->forceClose();
            return;
        }
        if (parseRes == 0)
        {
            break;
        }
        auto &req = requestParser->requestImpl();
        req->setPeerAddr(conn->peerAddr());
        req->setLocalAddr(conn->localAddr());
        req->setCreationDate(trantor::Date::date());
        req->setSecure(conn->isSSLConnection());
        req->setPeerCertificate(conn->peerCertificate());
        requests.push_back(req);
        requestParser->reset();
    }
    onRequests(conn, requests, requestParser);
    requests.clear();
}