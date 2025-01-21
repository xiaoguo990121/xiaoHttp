/**
 * @file AccessLogger.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 *
 */

#include <xiaoHttp/xiaoHttp.h>
#include <xiaoHttp/plugins/AccessLogger.h>
#include <xiaoHttp/plugins/RealIpResolver.h>
#include <regex>
#include <thread>
#if !defined _WIN32 && !defined __HAIKU__
#include <unistd.h>
#include <sys/syscall.h>
#elif defined __HAIKU__
#include <unistd.h>
#else
#include <sstream>
#endif

using namespace xiaoHttp;
using namespace xiaoHttp::plugin;

bool AccessLogger::useRealIp_ = false;

void AccessLogger::initAndStart(const Json::Value &config)
{
    useLocalTime_ = config.get("use_local_time", true).asBool();
    showMicroseconds_ = config.get("show_microseconds", true).asBool();
    timeFormat_ = config.get("custom_time_format", "").asString();
    useCustomTimeFormat_ = !timeFormat_.empty();
    useRealIp_ = config.get("use_real_ip", false).asBool();

    logFunctionMap_ = {{"$request_path", outputReqPath},
                       {"$path", outputReqPath},
                       {"$date",
                        [this](xiaoLog::LogStream &stream,
                               const xiaoHttp::HttpRequestPtr &req,
                               const xiaoHttp::HttpResponsePtr &resp)
                        {
                            outputDate(stream, req, resp);
                        }},
                       {"$request_date",
                        [this](xiaoLog::LogStream &stream,
                               const xiaoHttp::HttpRequestPtr &req,
                               const xiaoHttp::HttpResponsePtr &resp)
                        {
                            outputReqDate(stream, req, resp);
                        }},
                       {"$request_query", outputReqQuery},
                       {"$request_url", outputReqURL},
                       {"$query", outputReqQuery},
                       {"$url", outputReqURL},
                       {"$request_version", outputVersion},
                       {"$version", outputVersion},
                       {"$request", outputReqLine},
                       {"$remote_addr", outputRemoteAddr},
                       {"$local_addr", outputLocalAddr},
                       {"$request_len", outputReqLength},
                       {"$body_bytes_received", outputReqLength},
                       {"$method", outputMethod},
                       {"$thread", outputThreadNumber},
                       {"$response_len", outputRespLength},
                       {"$body_bytes_sent", outputRespLength},
                       {"$status", outputStatusString},
                       {"$status_code", outputStatusCode},
                       {"$processing_time", outputProcessingTime},
                       {"$upstream_http_content-type", outputRespContentType},
                       {"$upstream_http_content_type", outputRespContentType}};
    auto format = config.get("log_format", "").asString();
    if (format.empty())
    {
        format =
            "$request_data $method $url [$body_bytes_received] ($remote_addr- "
            "$local_addr) $status $body_bytes_sent $processing_time";
    }
    createLogFunctions(format);
    auto logPath = config.get("log_path", "").asString();
#ifdef XIAOHTTP_SPDLOG_SUPPORT
#endif
    if (!logPath.empty())
    {
        auto fileName = config.get("log_file", "access.log").asString();
        auto extension = std::string(".log");
        auto pos = fileName.rfind('.');
        if (pos != std::string::npos)
        {
            extension = fileName.substr(pos);
            fileName = fileName.substr(0, pos);
        }
        if (fileName.empty())
        {
            fileName = "access";
        }
        asyncFileLogger_.setFileName(fileName, extension, logPath);
        asyncFileLogger_.startLogging();
        logIndex_ = config.get("log_index", 0).asInt();
        xiaoLog::Logger::setOutputFunction(
            [&](const char *msg, const uint64_t len)
            {
                asyncFileLogger_.output(msg, len);
            },
            [&]()
            { asyncFileLogger_.flush(); },
            logIndex_);
        auto sizeLimit = config.get("log_size_limit", 0).asUInt64();
        if (sizeLimit > 0)
        {
            asyncFileLogger_.setFileSizeLimit(sizeLimit);
        }
        auto maxFiles = config.get("max_files", 0).asUInt();
        if (maxFiles >= 0)
        {
            asyncFileLogger_.setMaxFiles(maxFiles);
        }
    }
    xiaoHttp::app().registerPreSendingAdvice(
        [this](const xiaoHttp::HttpRequestPtr &req,
               const xiaoHttp::HttpResponsePtr &resp)
        {
            logging(LOG_RAW_TO(logIndex_), req, resp);
        });
}

void AccessLogger::shutdown()
{
}

void AccessLogger::logging(xiaoLog::LogStream &stream,
                           const xiaoHttp::HttpRequestPtr &req,
                           const xiaoHttp::HttpResponsePtr &resp)
{
    for (auto &func : logFunctions_)
    {
        func(stream, req, resp);
    }
}

void AccessLogger::createLogFunctions(std::string format)
{
    std::string rawString;
    while (!format.empty())
    {
        LOG_TRACE << format;
        auto pos = format.find('$');
        if (pos != std::string::npos)
        {
            rawString += format.substr(0, pos);

            format = format.substr(pos);
            std::regex e{"\\$[a-zA-Z0-9\\-_]+"};
            std::smatch m;
            if (std::regex_search(format, m, e))
            {
                if (!rawString.empty())
                {
                    logFunctions_.emplace_back(
                        [rawString](xiaoLog::LogStream &stream,
                                    const xiaoHttp::HttpRequestPtr &,
                                    const xiaoHttp::HttpResponsePtr &)
                        {
                            stream << rawString;
                        });
                    rawString.clear();
                }
                auto placeholder = m[0];
                logFunctions_.emplace_back(newLogFunction(placeholder));
                format = m.suffix().str();
            }
            else
            {
                rawString += '$';
                format = format.substr(1);
            }
        }
        else
        {
            rawString += format;
            break;
        }
    }
    if (!rawString.empty())
    {
        logFunctions_.emplace_back(
            [rawString =
                 std::move(rawString)](xiaoLog::LogStream &stream,
                                       const xiaoHttp::HttpRequestPtr &,
                                       const xiaoHttp::HttpResponsePtr &)
            {
                stream << rawString << "\n";
            });
    }
    else
    {
        logFunctions_.emplace_back(
            [](xiaoLog::LogStream &stream,
               const xiaoHttp::HttpRequestPtr &,
               const xiaoHttp::HttpResponsePtr &)
            {
                stream << "\n"
            });
    }
}

AccessLogger::LogFunction AccessLogger::newLogFunction(
    const std::string &placeholder)
{
    auto iter = logFunctionMap_.find(placeholder);
    if (iter != logFunctionMap_.end())
    {
        return iter->second;
    }
    if (placeholder.find("$http_") == 0 && placeholder.size() > 6)
    {
        auto headerName = placeholder.substr(6);
        return [headerName = std::move(headerName)](xiaoLog::LogStream &stream,
                                                    const xiaoHttp::HttpRequestPtr &req,
                                                    const xiaoHttp::HttpResponsePtr &)
        {
            outputReqHeader(stream, req, headerName);
        };
    }
    if (placeholder.find("$cookie_") == 0 && placeholder.size() > 8)
    {
        auto cookieName = placeholder.substr(8);
        return [cookieName = std::move(cookieName)](xiaoLog::LogStream &stream,
                                                    const xiaoHttp::HttpRequestPtr &req,
                                                    const xiaoHttp::HttpResponsePtr &)
        {
            outputReqCookie(stream, req, cookieName);
        };
    }
    if (placeholder.find("$upstream_http_") == 0 && placeholder.size() > 15)
    {
        auto headerName = placeholder.substr(15);
        return [headerName = std::move(headerName)](xiaoLog::LogStream &stream,
                                                    const xiaoHttp::HttpRequestPtr &,
                                                    const xiaoHttp::HttpResponsePtr &resp)
        {
            outputRespHeader(stream, resp, headerName);
        };
    }
    return [placeholder](xiaoLog::LogStream &stream,
                         const xiaoHttp::HttpRequestPtr &,
                         const xiaoHttp::HttpResponsePtr &)
    {
        stream << placeholder;
    };
}

void AccessLogger::ouputReqPath(xiaoLog::LogStream &stream,
                                const xiaoHttp::HttpRequestPtr &req,
                                const xiaoHttp::HttpResponsePtr &)
{
    stream << req->path();
}

void AccessLogger::outputDate(xiaoLog::LogStream &stream,
                              const xiaoHttp::HttpRequestPtr &,
                              const xiaoHttp::HttpResponsePtr &) const
{
    if (useCustomTimeFormat_)
    {
        if (useLocalTime_)
        {
            stream << xiaoLog::Date::now().toCustomedFormattedStringLocal(
                timeFormat_, showMicroseconds_);
        }
        else
        {
            stream << xiaoLog::Date::now().toCustomedFormattedString(
                timeFormat_, showMicroseconds_);
        }
    }
    else
    {
        if (useLocalTime_)
        {
            stream << xiaoLog::Date::now().toFormattedStringLocal(
                showMicroseconds_);
        }
        else
        {
            stream << xiaoLog::Date::now().toFormattedString(showMicroseconds_);
        }
    }
}

void AccessLogger::outputReqDate(xiaoLog::LogStream &stream,
                                 const xiaoHttp::HttpRequestPtr &req,
                                 const xiaoHttp::HttpResponsePtr &) const
{
    if (useCustomTimeFormat_)
    {
        if (useLocalTime_)
        {
            stream << req->creationDate().toCustomedFormattedStringLocal(
                timeFormat_, showMicroseconds_);
        }
        else
        {
            stream << req->creationDate().toCustomedFormattedString(
                timeFormat_, showMicroseconds_);
        }
    }
    else
    {
        if (useLocalTime_)
        {
            stream << req->creationDate().toFormattedStringLocal(
                showMicroseconds_);
        }
        else
        {
            stream << req->creationDate().toFormattedString(showMicroseconds_);
        }
    }
}

void AccessLogger::outputReqQuery(xiaoLog::LogStream &stream,
                                  const xiaoHttp::HttpRequestPtr &req,
                                  const xiaoHttp::HttpResponsePtr &)
{
    auto &query = req->query();
    if (query.empty())
    {
        stream << req->path();
    }
    else
    {
        stream << req->path() << "?" << query;
    }
}

void AccessLogger::outputVersion(xiaoLog::LogStream &stream,
                                 const xiaoHttp::HttpRequestPtr &req,
                                 const xiaoHttp::HttpResponsePtr &)
{
    stream << req->versionString();
}

void AccessLogger::outputReqLine(xiaoLog::LogStream &stream,
                                 const xiaoHttp::HttpRequestPtr &req,
                                 const xiaoHttp::HttpResponsePtr &)
{
    auto &query = req->query();
    if (query.empty())
    {
        stream << req->methodString() << " " << req->path() << " "
               << req->versionString();
    }
    else
    {
        stream << req->methodString() << " " << req->path() << '?' << query
               << " " << req->versionString();
    }
}

void AccessLogger::outputRemoteAddr(xiaoLog::LogStream &stream,
                                    const xiaoHttp::HttpRequestPtr &req,
                                    const xiaoHttp::HttpResponsePtr &)
{
    if (useRealIp_)
    {
        stream << RealIpResolver::GetRealAddr(req).toIpPort();
    }
    else
    {
        stream << req->peerAddr().toIpPort();
    }
}

void AccessLogger::outputLocalAddr(xiaoLog::LogStream &stream,
                                   const xiaoHttp::HttpRequestPtr &req,
                                   const xiaoHttp::HttpResponsePtr &)
{
    stream << req->localAddr().toIpPort();
}

void AccessLogger::outputReqLength(xiaoLog::LogStream &stream,
                                   const xiaoHttp::HttpRequestPtr &req,
                                   const xiaoHttp::HttpResponsePtr &)
{
    stream << req->body().length();
}

void AccessLogger::outputRespLength(xiaoLog::LogStream &stream,
                                    const xiaoHttp::HttpRequestPtr &req,
                                    const xiaoHttp::HttpResponsePtr &)
{
    stream << resp->body().length();
}

void AccessLogger::outputMethod(xiaoLog::LogStream &stream,
                                const xiaoHttp::HttpRequestPtr &req,
                                const xiaoHttp::HttpResponsePtr &)
{
    stream << req->methodString();
}

void AccessLogger::outputThreadNumber(xiaoLog::LogStream &stream,
                                      const xiaoHttp::HttpRequestPtr &,
                                      const xiaoHttp::HttpResponsePtr &)
{
#ifdef __linux__
    static thread_local pid_t threadId_{0};
#else
    static thread_local uint64_t threadId_{0};
#endif
#ifdef __linux__
    if (threadId_ == 0)
        threadId_ = static_cast<pid_t>(::syscall(SYS_gettid));
#elif defined __FreeBSD__
    if (threadId_ == 0)
    {
        threadId_ = pthread_getthreadid_np();
    }
#elif defined __OpenBSD__
    if (threadId_ == 0)
    {
        threadId_ = getthrid();
    }
#elif defined _WIN32 || defined __HAIKU__
    if (threadId_ == 0)
    {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        threadId_ = std::stoull(ss.str());
    }
#else
    if (threadId_ == 0)
    {
        pthread_threadid_np(NULL, &threadId_);
    }
#endif
    stream << threadId_;
}

void AccessLogger::outputReqHeader(xiaoLog::LogStream &stream,
                                   const xiaoHttp::HttpRequestPtr &req,
                                   const std::string &headerName)
{
    stream << headerName << ": " << req->getHeader(headerName);
}

void AccessLogger::outputReqCookie(xiaoLog::LogStream &stream,
                                   const xiaoHttp::HttpRequestPtr &req,
                                   const std::string &cookie)
{
    stream << "(cookie)" << cookie << "=" << req->getCookie(cookie);
}

void AccessLogger::outputRespHeader(xiaoLog::LogStream &stream,
                                    const xiaoHttp::HttpResponsePtr &resp,
                                    const std::string &headerName)
{
    stream << headerName << ": " << resp->getHeader(headerName);
}

void AccessLogger::outputStatusString(xiaoLog::LogStream &stream,
                                      const xiaoHttp::HttpRequestPtr &,
                                      const xiaoHttp::HttpResponsePtr &resp)
{
    int code = resp->getStatusCode();
    stream << code << " " << statusCodeToString(code);
}

void AccessLogger::outputStatusCode(xiaoLog::LogStream &stream,
                                    const xiaoHttp::HttpRequestPtr &,
                                    const xiaoHttp::HttpResponsePtr &resp)
{
    stream << resp->getStatusCode();
}

void AccessLogger::outputProcessingTime(xiaoLog::LogStream &stream,
                                        const xiaoHttp::HttpRequestPtr &req,
                                        const xiaoHttp::HttpResponsePtr &)
{
    auto start = req->creationDate();
    auto end = xiaoLog::Date::now();
    auto duration =
        end.microSecondsSinceEpoch() - start.microSecondsSinceEpoch();
    auto seconds = static_cast<double>(duration) / 1000000.0;
    stream << seconds;
}

void AccessLogger::outputRespContentType(xiaoLog::LogStream &stream,
                                         const xiaoHttp::HttpRequestPtr &,
                                         const xiaoHttp::HttpResponsePtr &resp)
{
    stream << resp->contentTypeString();
}