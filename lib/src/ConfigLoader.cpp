/**
 * @file ConfigLoader.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 *
 */

#include "ConfigLoader.h"
#include "HttpAppFrameworkImpl.h"
#include "ConfigAdapterManager.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include <xiaoHttp/utils/Utilities.h>

#include <xiaoLog/Logger.h>

#if !defined(_WIN32)
#include <unistd.h>
#define os_access access
#else
#endif

using namespace xiaoHttp;

static bool bytesSize(std::string &sizeStr, size_t &size)
{
    if (sizeStr.empty())
    {
        size = -1;
        return true;
    }
    else
    {
        size = 1;
        switch (sizeStr[sizeStr.length() - 1])
        {
        case 'k':
        case 'K':
            size = 1024;
            sizeStr.resize(sizeStr.length() - 1);
            break;
        case 'M':
        case 'm':
            size = (1024 * 1024);
            sizeStr.resize(sizeStr.length() - 1);
            break;
        case 'g':
        case 'G':
            size = (1024 * 1024 * 1024);
            sizeStr.resize(sizeStr.length() - 1);
            break;
#if ((ULONG_MAX) != (UINT_MAX))
        // 64bit system
        case 't':
        case 'T':
            size = (1024L * 1024L * 1024L * 1024L);
            sizeStr.resize(sizeStr.length() - 1);
            break;
#endif
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '7':
        case '8':
        case '9':
            break;
        default:
            return false;
            break;
        }
        std::istringstream iss(sizeStr);
        size_t tmpSize;
        iss >> tmpSize;
        if (iss.fail())
        {
            return false;
        }
        if ((size_t(-1) / tmpSize) >= size)
            size *= tmpSize;
        else
        {
            size = -1;
        }
        return true;
    }
}

ConfigLoader::ConfigLoader(const std::string &configFile)
{
    if (os_access(xiaoHttp::utils::toNativePath(configFile).c_str(), 0) != 0)
    {
        throw std::runtime_error("Config file " + configFile + " not found!");
    }
    if (os_access(xiaoHttp::utils::toNativePath(configFile).c_str(), R_OK) != 0)
    {
        throw std::runtime_error("No permission to read config file " +
                                 configFile);
    }
    configFile_ = configFile;
    auto pos = configFile.find_last_of('.');
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Invalid config file name!");
    }
    auto ext = configFile.substr(pos + 1);
    std::ifstream infile(xiaoHttp::utils::toNativePath(configFile).c_str(),
                         std::ifstream::in);
    // get the content of the infile
    std::string content((std::istreambuf_iterator<char>(infile)),
                        std::istream_iterator<char>());
    try
    {
        configJsonRoot_ =
            ConfigAdapterManager::instance().getJson(content, std::move(ext));
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Error reading config file " + configFile +
                                 ": " + e.what());
    }
}

ConfigLoader::ConfigLoader(const Json::Value &data) : configjsnRoot_(data)
{
}

ConfigLoader::ConfigLoader(Json::Value &&data)
    : configJsonRoot_(std::move(data))
{
}

ConfigLoader::~ConfigLoader()
{
}

static void loadLogSetting(const Json::Value &log)
{
    if (!log)
        return;
    auto useSpdlog = log.get("use_spdlog", false).asBool();
    auto loadPath = log.get("log_path", "").asString();
    auto baseName = log.get("logfile_base_name", "").asString();
    auto logSize = log.get("log_size_limit", 100000000).asUInt64();
    auto maxFiles = log.get("max_files", 0).asUInt();
    HttpAppFrameworkImpl::instance().setLogPath(
        loadPath, baseName, logSize, maxFiles, useSpdlog);
    auto logLevel = log.get("log_level", "DEBUG").asString();
    if (logLevel == "TRACE")
    {
        xiaoLog::Logger::setLogLevel(xiaoLog::Logger::kTrace);
    }
    else if (logLevel == "DEBUG")
    {
        xiaoLog::Logger::setLogLevel(xiaoLog::Logger::kDebug);
    }
    else if (logLevel == "INFO")
    {
        xiaoLog::Logger::setLogLevel(xiaoLog::Logger::kInfo);
    }
    else if (logLevel == "WARN")
    {
        xiaoLog::Logger::setLogLevel(xiaoLog::Logger::kWarn);
    }
    auto localTime = log.get("display_local_time", false).asBool();
    xiaoLog::Logger::setDisplayLocalTime(localtime);
}

static void loadControllers(const Json::Value &controllers)
{
    if (!controllers)
        return;
    for (auto const &controller : controllers)
    {
        auto path = controller.get("path", "").asString();
        auto ctrlName = controller.get("controller", "").asString();
        if (path == "" || ctrlName == "")
            continue;
        std::vector<internal::HttpConstraint> constraints;
        if (!controller["http_methods"].isNull())
        {
            for (auto const &method : controller["http_methods"])
            {
                auto strMethod = method.asString();
                std::transform(strMethod.begin(),
                               strMethod.end(),
                               strMethod.begin(),
                               [](unsigned char c)
                               { return tolower(c); });
                if (strMethod == "get")
                {
                    constraints.push_back(Get);
                }
                else if (strMethod == "post")
                {
                    constraints.push_back(Post);
                }
                else if (strMethod == "head") // The branch never work
                {
                    constraints.push_back(Head);
                }
                else if (strMethod == "put")
                {
                    constraints.push_back(Put);
                }
                else if (strMethod == "delete")
                {
                    constraints.push_back(Delete);
                }
                else if (strMethod == "patch")
                {
                    constraints.push_back(Patch);
                }
            }
        }
        if (!controller["filters"].isNull())
        {
            for (auto const &filter : controller["filters"])
            {
                constraints.push_back(filter.asString());
            }
        }
        xiaoHttp::app().registerHttpSimpleController(path, ctrlName, constraints);
    }
}

static void loadApp(const Json::Value &app)
{
    if (!app)
        return;
    // threads number
    auto threadsNum = app.get("thread_num", 1).asUInt64();
    if (threadsNum == 1)
    {
        threadsNum = app.get("number_of_threads", 1).asUInt64();
    }
    if (threadsNum == 0)
    {
        threadsNum = std::thread::hardware_concurrency();
        LOG_TRACE << "The number of processors is " << threadsNum;
    }
    if (threadsNum < 1)
        threadsNum = 1;
    xiaoHttp::app().setThreadNum(threadsNum);

    auto enableSession = app.get("enable_session", false).asBool();
    if (enableSession)
    {
        auto timeout = app.get("session_timeout", 0).asUInt64();
        auto sameSite = app.get("session_same_site", "Null").asString();
        auto cookieKey = app.get("session_cookie_key", "JSESSIONID").asString();
        auto maxAge = app.get("session_max_age", -1).asInt();
        xiaoHttp::app().enableSession(timeout,
                                      Cookie::convertString2SameSite(sameSite),
                                      cookieKey,
                                      maxAge);
    }
    else
        xiaoHttp::app().disableSession();

    // document root
    auto documentRoot = app.get("document_root", "").asString();
    if (documentRoot != "")
    {
        xiaoHttp::app().setDocumentRoot(documentRoot);
    }
    if (!app["static_file_headers"].empty())
    {
        if (app["static_file_headers"].isArray())
        {
            std::vector<std::pair<std::string, std::string>> headers;
            for (auto &header : app["static_file_headers"])
            {
                headers.emplace_back(
                    std::make_pair(header["name"].asString(),
                                   header["value"].asString()));
            }
            xiaoHttp::app().setStaticFileHeaders(headers);
        }
        else
        {
            throw std::runtime_error(
                "The static_file_headers option must be an array");
        }
    }

    // upload path
    auto uploadPath = app.get("upload_path", "uploads").asString();
    xiaoHttp::app().setUploadPath(uploadPath);
    // file types
    auto fileTypes = app["file_types"];
    if (fileTypes.isArray() && !fileTypes.empty())
    {
        std::vector<std::string> types;
        for (auto const &fileType : fileTypes)
        {
            types.push_back(fileType.asString());
            LOG_TRACE << "file type:" << types.back();
        }
        xiaoHttp::app().setFileTypes(types);
    }

    if (app.isMember("locations"))
    {
        auto &locations = app["locations"];
        if (!locations.isArray())
        {
            throw std::runtime_error("The locations option must be an array");
        }
        for (auto &location : locations)
        {
            auto uri = location.get("uri_prefix", "").asString();
            if (uri.empty())
                continue;
            auto defaultContentType =
                location.get("default_content_type", "").asString();
            auto alias = location.get("alias", "").asString();
            auto isCaseSensitive =
                location.get("is_case_sensitive", false).asBool();
            auto allAll = location.get("allow_all", true).asBool();
            auto isRecursive = location.get("is_recursive", true).asBool();
            if (!location["filters"].isNull())
            {
                if (location["filters"].isArray())
                {
                    std::vector<std::string> filters;
                    for (auto const &filter : location["filters"])
                    {
                        filters.push_back(filter.asString());
                    }
                    xiaoHttp::app().addALocation(uri,
                                                 defaultContentType,
                                                 alias,
                                                 isCaseSensitive,
                                                 allAll,
                                                 isRecursive,
                                                 filters);
                }
                else
                {
                    throw std::runtime_error("the filters of location '" + uri +
                                             "' should be an array");
                }
            }
            else
            {
                xiaoHttp::app().addALocation(uri,
                                             defaultContentType,
                                             alias,
                                             isCaseSensitive,
                                             allAll,
                                             isRecursive);
            }
        }
    }
    // max connections
    auto maxConns = app.get("max_connections", 0).asUInt64();
    if (maxConns > 0)
    {
        xiaoHttp::app().setMaxConnectionNum(maxConns);
    }
    // max connections per IP
    auto maxConnsPerIP = app.get("max_connections_per_ip", 0).asUInt64();
    if (maxConnsPerIP > 0)
    {
        xiaoHttp::app().setMaxConnectionNumPerIP(maxConnsPerIP);
    }
#ifndef _WIN32
    // dynamic views
    auto enableDynamicViews = app.get("load_dynamic_views", false).asBool();
    if (enableDynamicViews)
    {
        auto viewsPaths = app["dynamic_views_path"];
        if (viewsPaths.isArray() && viewsPaths.size() > 0)
        {
            std::vector<std::string> paths;
            for (auto const &viewsPath : viewsPaths)
            {
                paths.push_back(viewsPath.asString());
                LOG_TRACE << "views path:" << paths.back();
            }
            auto outputPath =
                app.get("dynamic_views_output_path", "").asString();
            xiaoHttp::app().enableDynamicViewsLoading(paths, outputPath);
        }
    }
#endif
    auto stackLimit = app.get("json_parser_stack_limit", 1000).asUInt64();
    xiaoHttp::app().setJsonParserStackLimit(stackLimit);
    auto unicodeEscaping =
        app.get("enable_unicode_escaping_in_json", true).asBool();
    xiaoHttp::app().setUnicodeEscapingInJson(unicodeEscaping);
    auto &precision = app["float_precision_in_json"];
    if (!precision.isNull())
    {
        auto precisionLength = precision.get("precision", 0).asUInt64();
        auto precisionType =
            precision.get("precision_type", "significant").asString();
        xiaoHttp::app().setFloatPrecisionInJson((unsigned int)precisionLength,
                                                precisionType);
    }
    // log
    loadLogSetting(app["log"]);
    // run as daemon
    auto runAsDaemon = app.get("run_as_daemon", false).asBool();
    if (runAsDaemon)
    {
        xiaoHttp::app().enableRunAsDaemon();
    }
    // handle SIGTERM
    auto handleSigterm = app.get("handle_sig_term", true).asBool();
    if (!handleSigterm)
    {
        xiaoHttp::app().disableSigtermHandling();
    }
    // relaunch
    auto relaunch = app.get("relaunch_on_error", false).asBool();
    if (relaunch)
    {
        xiaoHttp::app().enableRelaunchOnError();
    }
    auto useSendfile = app.get("use_sendfile", true).asBool();
    xiaoHttp::app().enableSendfile(useSendfile);
    auto useGzip = app.get("use_gzip", true).asBool();
    xiaoHttp::app().enableGzip(useGzip);
    auto useBr = app.get("use_brotli", false).asBool();
    xiaoHttp::app().enableBrotli(useBr);
    auto staticFilesCacheTime = app.get("static_files_cache_time", 5).asInt();
    xiaoHttp::app().setStaticFilesCacheTime(staticFilesCacheTime);
    loadControllers(app["simple_controllers_map"]);
    // Kick off idle connections
    auto kickOffTimeout = app.get("idle_connection_timeout", 60).asUInt64();
    xiaoHttp::app().setIdleConnectionTimeout(kickOffTimeout);
    auto server = app.get("server_header_field", "").asString();
    if (!server.empty())
        xiaoHttp::app().setServerHeaderField(server);
    auto sendServerHeader = app.get("enable_server_header", true).asBool();
    xiaoHttp::app().enableServerHeader(sendServerHeader);
    auto sendDateHeader = app.get("enable_date_header", true).asBool();
    xiaoHttp::app().enableDateHeader(sendDateHeader);
    auto keepaliveReqs = app.get("keepalive_requests", 0).asUInt64();
    xiaoHttp::app().setKeepaliveRequestsNumber(keepaliveReqs);
    auto pipeliningReqs = app.get("pipelining_requests", 0).asUInt64();
    xiaoHttp::app().setPipeliningRequestsNumber(pipeliningReqs);
    auto useGzipStatic = app.get("gzip_static", true).asBool();
    xiaoHttp::app().setGzipStatic(useGzipStatic);
    auto useBrStatic = app.get("br_static", true).asBool();
    xiaoHttp::app().setBrStatic(useBrStatic);
    auto maxBodySize = app.get("client_max_body_size", "1M").asString();
    size_t size;
    if (bytesSize(maxBodySize, size))
    {
        xiaoHttp::app().setClientMaxBodySize(size);
    }
    else
    {
        throw std::runtime_error("Error format of client_max_body_size");
    }
    auto maxMemoryBodySize =
        app.get("client_max_memory_body_size", "64K").asString();
    if (bytesSize(maxMemoryBodySize, size))
    {
        xiaoHttp::app().setClientMaxMemoryBodySize(size);
    }
    else
    {
        throw std::runtime_error("Error format of client_max_memory_body_size");
    }
    auto maxWsMsgSize =
        app.get("client_max_websocket_message_size", "128K").asString();
    if (bytesSize(maxWsMsgSize, size))
    {
        xiaoHttp::app().setClientMaxWebSocketMessageSize(size);
    }
    else
    {
        throw std::runtime_error(
            "Error format of client_max_websocket_message_size");
    }
    xiaoHttp::app().enableReusePort(app.get("reuse_port", false).asBool());
    xiaoHttp::app().setHomePage(app.get("home_page", "index.html").asString());
    xiaoHttp::app().setImplicitPageEnable(
        app.get("use_implicit_page", true).asBool());
    xiaoHttp::app().setImplicitPage(
        app.get("implicit_page", "index.html").asString());
    auto mimes = app["mime"];
    if (!mimes.isNull())
    {
        auto names = mimes.getMemberNames();
        for (const auto &mime : names)
        {
            auto ext = mimes[mime];
            std::vector<std::string> exts;
            if (ext.isString())
                exts.push_back(ext.asString());
            else if (ext.isArray())
            {
                for (const auto &extension : ext)
                    exts.push_back(extension.asString());
            }

            for (const auto &extension : exts)
                xiaoHttp::app().registerCustomExtensionMime(extension, mime);
        }
    }
    bool enableCompressedRequests =
        app.get("enabled_compressed_request", false).asBool();
    xiaoHttp::app().enableCompressedRequest(enableCompressedRequests);

    xiaoHttp::app().enableRequestStream(
        app.get("enable_request_stream", false).asBool());
}

void ConfigLoader::load()
{
    loadApp(configJsonRoot_["app"]);
}