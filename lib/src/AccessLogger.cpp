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
#include <regex>

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
                        [this](trantor::LogStream &stream,
                               const drogon::HttpRequestPtr &req,
                               const drogon::HttpResponsePtr &resp)
                        {
                            outputDate(stream, req, resp);
                        }},
                       {"$request_date",
                        [this](trantor::LogStream &stream,
                               const drogon::HttpRequestPtr &req,
                               const drogon::HttpResponsePtr &resp)
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