/**
 * @file HttpAppFramework.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-17
 *
 *
 */

#pragma once
#ifdef __cpp_impl_coroutine
#include <xiaoHttp/utils/coroutine.h>
#endif

#include <xiaoHttp/exports.h>
#include <xiaoHttp/utils/HttpConstraint.h>
#include <xiaoHttp/HttpBinder.h>
#include <xiaoHttp/HttpFilter.h>
#include <xiaoHttp/HttpRequest.h>
#include <xiaoHttp/HttpResponse.h>
#include <xiaoHttp/plugins/Plugin.h>
#include <xiaoHttp/xiaoHttp_callbacks.h>

#include <functional>

namespace xiaoHttp
{
    const char banner[] =
        " __  __  ___      _       ___  \n"
        " \\ \\/ / |_ _|    / \\     / _ \\ \n"
        "  \\  /   | |    / _ \\   | | | | \n"
        "  /  \\   | |   / ___ \\  | |_| |  \n"
        " /_/\\_\\ |___| /_/   \\_\\  \\___/  \n";

    XIAOHTTP_EXPORT std::string getVersion();
    XIAOHTTP_EXPORT std::string getGitCommit();

    class HttpControllerBase;
    class HttpSimpleControllerBase;
    class WebSocketControllerBase;
    using ExceptionHandler =
        std::function<void(const std::exception &,
                           const HttpRequestPtr &,
                           std::function<void(const HttpResponsePtr &)> &&)>;

    using DefaultHandler =
        std::function<void(const HttpRequestPtr &,
                           std::function<void(const HttpResponsePtr &)> &&)>;
    using HttpHandlerInfo = std::tuple<std::string, HttpMethod, std::string>;

#ifdef __cpp_impl_coroutine
    class HttpAppFramework;

    namespace internal
    {
        struct [[nodiscard]] ForwardAwaiter
            : public CallbackAwaiter<xiaoHttp::HttpResponsePtr>
        {
        public:
            ForwardAwaiter(xiaoHttp::HttpRequestPtr &&req,
                           std::string &&host,
                           double timeout,
                           HttpAppFramework &app)
                : req_(std::move(req)),
                  host_(std::move(host)),
                  timeout_(timeout),
                  app_(app)
            {
            }

            void await_suspend(std::coroutine_handle<> handle) noexcept;

        private:
            xiaoHttp::HttpRequestPtr req_;
            std::string host_;
            double timeout_;
            HttpAppFramework &app_;
        };
    }
#endif

    class XIAOHTTP_EXPORT HttpAppFramework : public xiaoNet::NonCopyable
    {
    public:
        virtual ~HttpAppFramework() = default;

        static HttpAppFramework &instance();

        virtual void run() = 0;

        virtual bool isRunning() = 0;

        virtual void quit() = 0;

        virtual xiaoNet::EventLoop *getLoop() const = 0;

        virtual xiaoNet::EventLoop *getIOLoop(size_t id) const = 0;

        virtual HttpAppFramework &setCustom404Page(const HttpResponsePtr &resp,
                                                   bool set404 = true) = 0;

        virtual HttpAppFramework &setCustomErrorHandler(
            std::function<HttpResponsePtr(HttpStatusCode,
                                          const HttpRequestPtr &req)>
                &&resp_generator) = 0;

        HttpAppFramework &setCustomErrorHandler(
            std::function<HttpResponsePtr(HttpStatusCode)> &&resp_generator)
        {
            return setCustomErrorHandler(
                [cb = std::move(resp_generator)](HttpStatusCode code,
                                                 const HttpRequestPtr &)
                {
                    return cb(code);
                });
        }

        virtual const std::function<HttpResponsePtr(HttpStatusCode,
                                                    const HttpRequestPtr &req)> &
        getCustomErrorHandler() const = 0;

        template <typename T>
        T *getPlugin()
        {
            static_assert(IsPlugin<T>::value,
                          "The Template parameter must be a subclass of "
                          "PluginBase");
            assert(isRunning());
            static auto pluginPtr =
                dynamic_cast<T *>(getPlugin(T::classTypeName()));
            return pluginPtr;
        }

        template <typename T>
        std::shared_ptr<T> getSharedPlugin()
        {
            static_assert(IsPlugin<T>::value,
                          "The Template parameter must be a subclass of "
                          "PluginBase");
            assert(isRunning());
            static auto pluginPtr =
                std::dynamic_pointer_cast<T>(getSharedPlugin(T::classTypeName()));
            return pluginPtr;
        }

        virtual PluginBase *getPlugin(const std::string &name) = 0;

        virtual std::shared_ptr<PluginBase> getSharedPlugin(
            const std::string &name) = 0;

        virtual HttpAppFramework &registerBeginningAdvice(
            const std::function<void()> &advice) = 0;

        virtual HttpAppFramework &registerNewConnectionAdvice(
            const std::function<bool(const xiaoNet::InetAddress &,
                                     const xiaoNet::InetAddress &)> &advice) = 0;
        virtual HttpAppFramework &registerHttpResponseCreationAdvice(
            const std::function<void(const HttpResponsePtr &)> &advice) = 0;

        virtual HttpAppFramework &registerSyncAdvice(
            const std::function<HttpResponsePtr(const HttpRequestPtr &)>
                &advice) = 0;

        virtual HttpAppFramework &registerPreRoutingAdvice(
            const std::function<void(const HttpRequestPtr &,
                                     AdviceCallback &&,
                                     AdviceChainCallback &&)> &advice) = 0;

        virtual HttpAppFramework &registerPreRoutingAdvice(
            const std::function<void(const HttpRequestPtr &)> &advice) = 0;

        virtual HttpAppFramework &registerPostRoutingAdvice(
            const std::function<void(const HttpRequestPtr &,
                                     AdviceCallback &&,
                                     AdviceChainCallback &&)> &advice) = 0;

        virtual HttpAppFramework &registerPostRoutingAdvice(
            const std::function<void(const HttpRequestPtr &)> &advice) = 0;

        virtual HttpAppFramework &registerPreHandlingAdvice(
            const std::function<void(const HttpRequestPtr &,
                                     AdviceCallback &&,
                                     AdviceChainCallback &&)> &advice) = 0;

        virtual HttpAppFramework &registerPreHandlingAdvice(
            const std::function<void(const HttpRequestPtr &)> &advice) = 0;

        virtual HttpAppFramework &registerPostHandlingAdvice(
            const std::function<void(const HttpRequestPtr &,
                                     const HttpResponsePtr &)> &advice) = 0;

        virtual HttpAppFramework &registerPreSendingAdvice(
            const std::function<void(const HttpRequestPtr &,
                                     const HttpResponsePtr &)> &advice) = 0;

        virtual HttpAppFramework &setupFileLogger() = 0;

        virtual HttpAppFramework &loadConfigFile(
            const std::string &fileName) noexcept(false) = 0;

        virtual HttpAppFramework &loadConfigJson(const Json::Value &data) noexcept(
            false) = 0;

        virtual HttpAppFramework &loadConfigJson(Json::Value &&data) noexcept(
            false) = 0;

        virtual HttpAppFramework &registerHttpSimpleController(
            const std::string &pathName,
            const std::string &ctrlName,
            const std::vector<internal::HttpConstraint> &filtersAndMethods =
                std::vector<internal::HttpConstraint>{}) = 0;

        template <typename FUNCTION>
        HttpAppFramework &registerHandler(
            const std::string &pathPattern,
            FUNCTION &&function,
            const std::vector<internal::HttpConstraint> &filtersAndMethods =
                std::vector<internal::HttpConstraint>{},
            const std::string &handlerName = "")
        {
            LOG_TRACE << "pathPattern:" << pathPattern;
            auto binder = std::make_shared<internal::HttpBinder<FUNCTION>>(
                std::forward<FUNCTION>(function));

            getLoop()->queueInLoop([binder]()
                                   { binder->createHandlerInstance(); });

            std::vector<HttpMethod> validMethods;
            std::vector<std::string> filters;
            for (auto const &filterOrMethod : filtersAndMethods)
            {
                if (filterOrMethod.type() == internal::ConstraintType::HttpFilter)
                {
                    filters.push_back(filterOrMethod.getFilterName());
                }
                else if (filterOrMethod.type() ==
                         internal::ConstraintType::HttpMethod)
                {
                    validMethods.push_back(filterOrMethod.getHttpMethod());
                }
                else
                {
                    LOG_ERROR << "Invalid controller constraint type";
                    exit(1);
                }
            }
            registerHttpController(
                pathPattern, binder, validMethods, filters, handlerName);
            return *this;
        }

        template <typename FUNCTION>
        HttpAppFramework &registerHandlerViaRegex(
            const std::string &regExp,
            FUNCTION &&function,
            const std::vector<internal::HttpConstraint> &filtersAndMethods =
                std::vector<internal::HttpConstraint>{},
            const std::string &handlerName = "")
        {
            LOG_TRACE << "regex:" << regExp;
            internal::HttpBinderBasePtr binder;

            binder = std::make_shared<internal::HttpBinder<FUNCTION>>(
                std::forward<FUNCTION>(function));

            std::vector<HttpMethod> validMethods;
            std::vector<std::string> filters;
            for (auto const &filterOrMethod : filtersAndMethods)
            {
                if (filterOrMethod.type() == internal::ConstraintType::HttpFilter)
                {
                    filters.push_back(filterOrMethod.getFilterName());
                }
                else if (filterOrMethod.type() ==
                         internal::ConstraintType::HttpMethod)
                {
                    validMethods.push_back(filterOrMethod.getHttpMethod());
                }
                else
                {
                    LOG_ERROR << "Invalid controller constraint type";
                    exit(1);
                }
            }
            registerHttpControllerViaRegex(
                regExp, binder, validMethods, filters, handlerName);
            return *this;
        }

        virtual HttpAppFramework &registerWebSocketController(
            const std::string &pathName,
            const std::string &ctrlName,
            const std::vector<internal::HttpConstraint> &filtersAndMethods =
                std::vector<internal::HttpConstraint>{}) = 0;

        virtual HttpAppFramework &registerWebSocketControllerRegex(
            const std::string &regExp,
            const std::string &ctrlName,
            const std::vector<internal::HttpConstraint> &constraints =
                std::vector<internal::HttpConstraint>{}) = 0;

        template <typename T>
        HttpAppFramework &registerController(const std::shared_ptr<T> &ctrlPtr)
        {
            static_assert((std::is_base_of<HttpControllerBase, T>::value ||
                           std::is_base_of<HttpSimpleControllerBase, T>::value ||
                           std::is_base_of<WebSocketControllerBase, T>::value),
                          "Error! Only controller objects can be registered here");
            static_assert(!T::isAutoCreation,
                          "Controllers created and initialized "
                          "automatically by drogon cannot be "
                          "registered here");
            DrClassMap::setSingleInstance(ctrlPtr);
            T::initPathRouting();
            return *this;
        }

        template <typename T>
        HttpAppFramework &registerFilter(const std::shared_ptr<T> &filterPtr)
        {
            static_assert(std::is_base_of<HttpFilterBase, T>::value,
                          "Error! Only filter objects can be registered here");
            static_assert(!T::isAutoCreation,
                          "Filters created and initialized "
                          "automatically by drogon cannot be "
                          "registered here");
            DrClassMap::setSingleInstance(filterPtr);
            return *this;
        }

        virtual HttpAppFramework &setDefaultHandler(DefaultHandler handler) = 0;

        virtual void forward(
            const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string &hostString = "",
            double timeout = 0) = 0;
#ifdef __cpp_impl_coroutine
        /**
         * @brief Forward the http request, this is the coroutine version of the
         * above method.
         */
        internal::ForwardAwaiter forwardCoro(HttpRequestPtr req,
                                             std::string hostString = "",
                                             double timeout = 0)
        {
            return internal::ForwardAwaiter(std::move(req),
                                            std::move(hostString),
                                            timeout,
                                            *this);
        }
#endif
        /// Get information about the handlers registered to drogon
        /**
         * @return
         * The first item of std::tuple in the return value represents the path
         * pattern of the handler;
         * The last item in std::tuple is the description of the handler.
         */
        virtual std::vector<HttpHandlerInfo> getHandlersInfo() const = 0;

        /// Get the custom configuration defined by users in the configuration file.
        virtual const Json::Value &getCustomConfig() const = 0;

        /// Set the number of threads for IO event loops
        /**
         * @param threadNum the number of threads
         * The default value is 1, if the parameter is 0, the number is equal to
         * the number of CPU cores.
         *
         * @note
         * This number is usually less than or equal to the number of CPU cores.
         * This number can be configured in the configuration file.
         */
        virtual HttpAppFramework &setThreadNum(size_t threadNum) = 0;

        /// Get the number of threads for IO event loops
        virtual size_t getThreadNum() const = 0;

        /// Set the global cert file and private key file for https
        /// These options can be configured in the configuration file.
        virtual HttpAppFramework &setSSLFiles(const std::string &certPath,
                                              const std::string &keyPath) = 0;

        /// Supplies file style SSL options to `SSL_CONF_cmd`. Valid options are
        /// available at
        /// https://www.openssl.org/docs/manmaster/man3/SSL_CONF_cmd.html
        virtual HttpAppFramework &setSSLConfigCommands(
            const std::vector<std::pair<std::string, std::string>>
                &sslConfCmds) = 0;

        /// Add plugins
        /**
         * @param configs The plugins array
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual void addPlugins(const Json::Value &configs) = 0;

        /// Add a plugin
        /**
         * @param name Name of the plugin
         * @param dependencies Names of plugins this plugin depends on
         * @param config Custom config for the plugin
         */
        virtual void addPlugin(const std::string &name,
                               const std::vector<std::string> &dependencies,
                               const Json::Value &config) = 0;

        /// Add a listener for http or https service
        /**
         * @param ip is the ip that the listener listens on.
         * @param port is the port that the listener listens on.
         * @param useSSL if the parameter is true, the listener is used for the
         * https service.
         * @param certFile
         * @param keyFile specify the cert file and the private key file for this
         * listener. If they are empty, the global configuration set by the above
         * method is used.
         * @param useOldTLS if true, the TLS1.0/1.1 are enabled for HTTPS
         * connections.
         * @param sslConfCmds vector of ssl configuration key/value pairs.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &addListener(
            const std::string &ip,
            uint16_t port,
            bool useSSL = false,
            const std::string &certFile = "",
            const std::string &keyFile = "",
            bool useOldTLS = false,
            const std::vector<std::pair<std::string, std::string>> &sslConfCmds =
                {}) = 0;

        /// Enable sessions supporting.
        /**
         * @param timeout The number of seconds which is the timeout of a session
         * @param sameSite The default value of SameSite attribute
         * @param cookieKey The key of the session cookie
         *
         * @note
         * Session support is disabled by default.
         * If there isn't any request from a client for timeout(>0) seconds,
         * the session of the client is destroyed.
         * If the timeout parameter is equal to 0, sessions will remain permanently
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &enableSession(
            const size_t timeout = 0,
            Cookie::SameSite sameSite = Cookie::SameSite::kNull,
            const std::string &cookieKey = "JSESSIONID",
            int maxAge = -1,
            std::function<std::string()> idGeneratorCallback = nullptr) = 0;

        /// A wrapper of the above method.
        /**
         *   Example: Users can set the timeout value as follows:
         * @code
            app().enableSession(0.2h);
            app().enableSession(12min);
           @endcode
         */
        inline HttpAppFramework &enableSession(
            const std::chrono::duration<double> &timeout,
            Cookie::SameSite sameSite = Cookie::SameSite::kNull,
            const std::string &cookieKey = "JSESSIONID",
            int maxAge = -1,
            std::function<std::string()> idGeneratorCallback = nullptr)
        {
            return enableSession((size_t)timeout.count(),
                                 sameSite,
                                 cookieKey,
                                 maxAge,
                                 idGeneratorCallback);
        }

        /// Register an advice called when starting a new session.
        /**
         * @param advice is called with the session id.
         */
        virtual HttpAppFramework &registerSessionStartAdvice(
            const AdviceStartSessionCallback &advice) = 0;

        /// Register an advice called when destroying a session.
        /**
         * @param advice is called with the session id.
         */
        virtual HttpAppFramework &registerSessionDestroyAdvice(
            const AdviceDestorySessionCallback &advice) = 0;

        /// Disable sessions supporting.
        /**
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &disableSession() = 0;

        /// Set the root path of HTTP document, default path is ./
        /**
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setDocumentRoot(const std::string &rootPath) = 0;

        /// Get the document root directory.
        virtual const std::string &getDocumentRoot() const = 0;
        /**
         * @brief Set the Static File Headers
         *
         * @param headers Each pair object in the vector presents the field name and
         * field value of a header in an static file response.
         */
        virtual HttpAppFramework &setStaticFileHeaders(
            const std::vector<std::pair<std::string, std::string>> &headers) = 0;

        /**
         * @brief Add a location of static files for GET requests.
         *
         * @param uriPrefix The URI prefix of the location prefixed with "/"
         * @param defaultContentType The default content type of the static files
         * without an extension.
         * @param alias The location in file system, if it is prefixed with "/", it
         * presents an absolute path, otherwise it presents a relative path to the
         * document_root path.
         * @param isCaseSensitive
         * @param allowAll If it is set to false, only static files with a valid
         * extension can be accessed.
         * @param isRecursive If it is set to false, files in sub directories can't
         * be accessed.
         * @param filters The list of filters which acting on the location.
         * @return HttpAppFramework&
         */
        virtual HttpAppFramework &addALocation(
            const std::string &uriPrefix,
            const std::string &defaultContentType = "",
            const std::string &alias = "",
            bool isCaseSensitive = false,
            bool allowAll = true,
            bool isRecursive = true,
            const std::vector<std::string> &filters = {}) = 0;

        /// Set the path to store uploaded files.
        /**
         * @param uploadPath is the directory where the uploaded files are
         * stored. if it isn't prefixed with /, ./ or ../, it is relative path
         * of document_root path, The default value is 'uploads'.
         *
         * @note
         * This operation can be performed by an option in the configuration
         * file.
         */
        virtual HttpAppFramework &setUploadPath(const std::string &uploadPath) = 0;

        /// Get the path to store uploaded files.
        virtual const std::string &getUploadPath() const = 0;

        /// Set types of files that can be downloaded.
        /**
         *   Example:
         * @code
           app.setFileTypes({"html","txt","png","jpg"});
           @endcode
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setFileTypes(
            const std::vector<std::string> &types) = 0;

#ifndef _WIN32
        /// Enable supporting for dynamic views loading.
        /**
         *
         * @param libPaths is a vector that contains paths to view files.
         *
         * @param outputPath is the directory where the output source files locate.
         * If it is set to an empty string, drogon use libPaths as output paths. If
         * the path isn't prefixed with /, it is the relative path of the current
         * working directory.
         *
         * @note
         * It is disabled by default.
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &enableDynamicViewsLoading(
            const std::vector<std::string> &libPaths,
            const std::string &outputPath = "") = 0;
#endif

        /// Set the maximum number of all connections.
        /**
         * The default value is 100000.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setMaxConnectionNum(size_t maxConnections) = 0;

        /// Set the maximum number of connections per remote IP.
        /**
         * The default value is 0 which means no limit.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setMaxConnectionNumPerIP(
            size_t maxConnectionsPerIP) = 0;

        /// Make the application run as a daemon.
        /**
         * Disabled by default.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &enableRunAsDaemon() = 0;

        /// Disable the handling of SIGTERM signal.
        /**
         * Enabled by default.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         * When disabled setTermSignalHandler() is useless
         */
        virtual HttpAppFramework &disableSigtermHandling() = 0;

        /// Make the application restart after crashing.
        /**
         * Disabled by default.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &enableRelaunchOnError() = 0;

        /**
         * @brief Set the output path of logs.
         * @param logPath The path to logs - logs to console if empty.
         * @param logfileBaseName The base name of log files - defaults to "drogon"
         * if empty.
         * @param logSize indicates the maximum size of a log file.
         * @param maxFiles max count of log file - 0 = unlimited.
         * @param useSpdlog Use spdlog for logging (if compiled-in).
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setLogPath(
            const std::string &logPath,
            const std::string &logfileBaseName = "",
            size_t logSize = 100000000,
            size_t maxFiles = 0,
            bool useSpdlog = false) = 0;

        /**
         * @brief Set the log level.
         * @param level is one of TRACE, DEBUG, INFO, WARN. The Default value is
         * DEBUG.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setLogLevel(xiaoLog::Logger::LogLevel level) = 0;

        /// Set the log time display
        /**
         * @param on is true to display local time, false to display UTC time. The
         * Default value is false.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setLogLocalTime(bool on) = 0;

        /// Enable the sendfile system call in linux.
        /**
         * @param sendFile if the parameter is true, sendfile() system-call is used
         * to send static files to clients; The default value is true.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         * Even though sendfile() is enabled, only files larger than 200k are sent
         * this way,
         * because the advantages of sendfile() can only be reflected in sending
         * large files.
         */
        virtual HttpAppFramework &enableSendfile(bool sendFile) = 0;

        /// Enable gzip compression.
        /**
         * @param useGzip if the parameter is true, use gzip to compress the
         * response body's content;
         * The default value is true.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         * After gzip is enabled, gzip is used under the following conditions:
         * 1. The content type of response is not a binary type.
         * 2. The content length is bigger than 1024 bytes.
         */
        virtual HttpAppFramework &enableGzip(bool useGzip) = 0;

        /// Return true if gzip is enabled.
        virtual bool isGzipEnabled() const = 0;

        /// Enable brotli compression.
        /**
         * @param useBrotli if the parameter is true, use brotli to compress the
         * response body's content;
         * The default value is true.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         * After brotli is enabled, brotli is used under the following conditions:
         * 1. The content type of response is not a binary type.
         * 2. The content length is bigger than 1024 bytes.
         */
        virtual HttpAppFramework &enableBrotli(bool useBrotli) = 0;

        /// Return true if brotli is enabled.
        virtual bool isBrotliEnabled() const = 0;

        /// Set the time in which the static file response is cached in memory.
        /**
         * @param cacheTime in seconds. 0 means always cached, negative means no
         * cache
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setStaticFilesCacheTime(int cacheTime) = 0;

        /// Get the time set by the above method.
        virtual int staticFilesCacheTime() const = 0;

        /// Set the lifetime of the connection without read or write
        /**
         * @param timeout in seconds. 60 by default. Setting the timeout to 0 means
         * that drogon does not close idle connections.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setIdleConnectionTimeout(size_t timeout) = 0;

        /// A wrapper of the above method.
        /**
         *   Example:
         * Users can set the timeout value as follows:
         * @code
             app().setIdleConnectionTimeout(0.5h);
             app().setIdleConnectionTimeout(30min);
           @endcode
         */
        inline HttpAppFramework &setIdleConnectionTimeout(
            const std::chrono::duration<double> &timeout)
        {
            return setIdleConnectionTimeout((size_t)timeout.count());
        }

        /// Set the 'server' header field in each response sent by drogon.
        /**
         * @param server empty string by default with which the 'server' header
         * field is set to "Server: drogon/version string\r\n"
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setServerHeaderField(
            const std::string &server) = 0;

        /// Control if the 'Server' header is added to each HTTP response.
        /**
         * @note
         * These operations can be performed by options in the configuration file.
         * The headers are sent to clients by default.
         */
        virtual HttpAppFramework &enableServerHeader(bool flag) = 0;

        /// Control if the 'Date' header is added to each HTTP response.
        /**
         * @note
         * These operations can be performed by options in the configuration file.
         * The headers are sent to clients by default.
         */
        virtual HttpAppFramework &enableDateHeader(bool flag) = 0;

        /// Set the maximum number of requests that can be served through one
        /// keep-alive connection.
        /**
         * After the maximum number of requests are made, the connection is closed.
         * The default value is 0 which means no limit.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setKeepaliveRequestsNumber(
            const size_t number) = 0;

        /// Set the maximum number of unhandled requests that can be cached in
        /// pipelining buffer.
        /**
         * The default value of 0 means no limit.
         * After the maximum number of requests cached in pipelining buffer are
         * made, the connection is closed.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setPipeliningRequestsNumber(
            const size_t number) = 0;

        /// Set the gzip_static option.
        /**
         * If it is set to true, when the client requests a static file, drogon
         * first finds the compressed file with the extension ".gz" in the same path
         * and send the compressed file to the client. The default value is true.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setGzipStatic(bool useGzipStatic) = 0;

        /// Set the br_static option.
        /**
         * If it is set to true, when the client requests a static file, drogon
         * first finds the compressed file with the extension ".br" in the same path
         * and send the compressed file to the client. The default value is true.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setBrStatic(bool useGzipStatic) = 0;

        /// Set the max body size of the requests received by drogon.
        /**
         * The default value is 1M.
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setClientMaxBodySize(size_t maxSize) = 0;

        /// Set the maximum body size in memory of HTTP requests received by drogon.
        /**
         * The default value is "64K" bytes. If the body size of a HTTP request
         * exceeds this limit, the body is stored to a temporary file for
         * processing.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setClientMaxMemoryBodySize(size_t maxSize) = 0;

        /// Set the max size of messages sent by WebSocket client.
        /**
         * The default value is 128K.
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setClientMaxWebSocketMessageSize(
            size_t maxSize) = 0;

        // Set the HTML file of the home page, the default value is "index.html"
        /**
         * If there isn't any handler registered to the path "/", the home page file
         * in the "document_root"
         * is send to clients as a response to the request for "/".
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setHomePage(const std::string &homePageFile) = 0;

        /**
         * @brief Set the TERM Signal Handler. This method provides a way to users
         * for exiting program gracefully. When the TERM signal is received after
         * app().run() is called, the handler is invoked. Drogon uses a default
         * signal handler for the TERM signal, which calls the 'app().quit()' method
         * when the TERM signal is received.
         *
         * @param handler
         * @return HttpAppFramework&
         */
        virtual HttpAppFramework &setTermSignalHandler(
            const std::function<void()> &handler) = 0;

        /**
         * @brief Set the INT Signal Handler. This method provides a way to users
         * for exiting program gracefully. When the INT signal is received after
         * app().run() is called, the handler is invoked. Drogon uses a default
         * signal handler for the INT signal, which calls the 'app().quit()' method
         * when the INT signal is received.
         *
         * @param handler
         * @return HttpAppFramework&
         */
        virtual HttpAppFramework &setIntSignalHandler(
            const std::function<void()> &handler) = 0;

        /// Get homepage, default is "index.html"
        /**
         * @note
         * This method must be called after the framework has been run.
         */
        virtual const std::string &getHomePage() const = 0;

        /// Set to enable implicit pages, enabled by default
        /**
         * @brief Implicit pages are used when the server detects if the user
         * requested a directory. By default, it will try to append index.html to
         * the path, see setImplicitPage() if you want to customize this
         * (http://localhost/a-directory resolves to
         * http://localhost/a-directory/index.html by default).
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setImplicitPageEnable(bool useImplicitPage) = 0;

        /// Return true if implicit pages are enabled
        /**
         * @note
         * This method must be called after the framework has been run.
         */
        virtual bool isImplicitPageEnabled() const = 0;

        /// Set the HTML file that a directory would resolve to by default, default
        /// is "index.html"
        /**
         * @brief Set the page which would the server load in if it detects that
         * the user requested a directory
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setImplicitPage(
            const std::string &implicitPageFile) = 0;

        /// Get the implicit HTML page
        /**
         * @note
         * This method must be called after the framework has been run.
         */
        virtual const std::string &getImplicitPage() const = 0;

        /// Get a database client by name
        /**
         * @note
         * This method must be called after the framework has been run.
         */
        // virtual orm::DbClientPtr getDbClient(
        //     const std::string &name = "default") = 0;

        /// Get a 'fast' database client by name
        /**
         * @note
         * This method must be called after the framework has been run.
         */
        // virtual orm::DbClientPtr getFastDbClient(
        //     const std::string &name = "default") = 0;

        /**
         * @brief Check if all database clients in the framework are available
         * (connect to the database successfully).
         */
        virtual bool areAllDbClientsAvailable() const noexcept = 0;

        /// Get a redis client by name
        /**
         * @note
         * This method must be called after the framework has been run.
         */
        // virtual nosql::RedisClientPtr getRedisClient(
        //     const std::string &name = "default") = 0;

        /// Get a 'fast' redis client by name
        /**
         * @note
         * This method must be called after the framework has been run.
         */
        // virtual nosql::RedisClientPtr getFastRedisClient(
        //     const std::string &name = "default") = 0;

        /**
         * @brief Set the maximum stack depth of the json parser when reading a json
         * string, the default value is 1000.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &setJsonParserStackLimit(
            size_t limit) noexcept = 0;

        /**
         * @brief Get the maximum stack depth of the json parser when reading a json
         * string.
         */
        virtual size_t getJsonParserStackLimit() const noexcept = 0;
        /**
         * @brief This method is to enable or disable the unicode escaping (\\u) in
         * the json string of HTTP responses or requests. it works (disable
         * successfully) when the version of JsonCpp >= 1.9.3, the unicode escaping
         * is enabled by default.
         */
        virtual HttpAppFramework &setUnicodeEscapingInJson(
            bool enable) noexcept = 0;

        /**
         * @brief Check if the unicode escaping is used in the json string of HTTP
         * requests and responses.
         */
        virtual bool isUnicodeEscapingUsedInJson() const noexcept = 0;

        /**
         * @brief Set the float precision in Json string of HTTP requests or
         * responses with json content.
         *
         * @param precision The maximum digits length.
         * @param precisionType Must be "significant" or "decimal", defaults to
         * "significant" that means setting max number of significant digits in
         * string, "decimal" means setting max number of digits after "." in string
         * @return HttpAppFramework&
         */
        virtual HttpAppFramework &setFloatPrecisionInJson(
            unsigned int precision,
            const std::string &precisionType = "significant") noexcept = 0;
        /**
         * @brief Get the float precision set by the above method.
         *
         * @return std::pair<size_t, std::string>
         */
        virtual const std::pair<unsigned int, std::string> &
        getFloatPrecisionInJson() const noexcept = 0;
        /// Create a database client
        /**
         * @param dbType The database type is one of
         * "postgresql","mysql","sqlite3".
         * @param host IP or host name.
         * @param port The port on which the database server is listening.
         * @param databaseName Database name
         * @param userName User name
         * @param password Password for the database server
         * @param connectionNum The number of connections to the database server.
         * It's valid only if @p isFast is false.
         * @param filename The file name of sqlite3 database file.
         * @param name The client name.
         * @param isFast Indicates if the client is a fast database client.
         * @param characterSet The character set of the database server.
         * @param timeout The timeout in seconds for executing SQL queries. zero or
         * negative value means no timeout.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &createDbClient(
            const std::string &dbType,
            const std::string &host,
            const unsigned short port,
            const std::string &databaseName,
            const std::string &userName,
            const std::string &password,
            const size_t connectionNum = 1,
            const std::string &filename = "",
            const std::string &name = "default",
            const bool isFast = false,
            const std::string &characterSet = "",
            double timeout = -1.0,
            const bool autoBatch = false) = 0;

        /// Create a redis client
        /**
         * @param ip IP of redis server.
         * @param port The port on which the redis server is listening.
         * @param name The client name.
         * @param username Username for redis server
         * @param password Password for the redis server
         * @param connectionNum The number of connections to the redis server.
         * @param isFast Indicates if the client is a fast database client.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual HttpAppFramework &createRedisClient(
            const std::string &ip,
            unsigned short port,
            const std::string &name = "default",
            const std::string &password = "",
            size_t connectionNum = 1,
            bool isFast = false,
            double timeout = -1.0,
            unsigned int db = 0,
            const std::string &username = "") = 0;

        /// Get the DNS resolver
        /**
         * @note
         * When the c-ares library is installed in the system, it runs with the best
         * performance.
         */
        // virtual const std::shared_ptr<trantor::Resolver> &getResolver() const = 0;

        /// Return true is drogon supports SSL(https)
        virtual bool supportSSL() const = 0;

        /**
         * @brief Get the Current Thread Index whose range is [0, the total number
         * of IO threads]
         *
         * @return size_t If the current thread is the main EventLoop thread (in
         * which the app().run() is called), the number of the IO threads is
         * returned. If the current thread is a network IO thread, the index of it
         * in the range [0, the number of IO threads) is returned. otherwise the
         * maximum value of type size_t is returned.
         *
         * @note Basically this method is used for storing thread-related various in
         * an array and users can use indexes returned by this method to access
         * them. This is much faster than using a map. If the array is properly
         * initialized at the beginning, users can access it without locks.
         */
        virtual size_t getCurrentThreadIndex() const = 0;

        /**
         * @brief Get the addresses of listeners.
         *
         * @return std::vector<trantor::InetAddress>
         * @note This method should be called after calling the app().run(). One
         * could run this method in an AOP join point (such as the BeginningAdvice).
         */
        virtual std::vector<xiaoNet::InetAddress> getListeners() const = 0;

        /**
         * @brief Enable ReusePort mode or not. If the mode is enabled, one can run
         * multiple processes listening to the same port at the same time. If this
         * method is not called, the feature is disabled.
         *
         * @note
         * This operation can be performed by an option in the configuration file.
         */
        virtual void enableReusePort(bool enable = true) = 0;

        /**
         * @brief Return if the ReusePort mode is enabled.
         */
        virtual bool reusePort() const = 0;

        /**
         * @brief handler will be called upon an exception escapes a request handler
         */
        virtual HttpAppFramework &setExceptionHandler(ExceptionHandler handler) = 0;

        /**
         * @brief returns the excaption handler
         */
        virtual const ExceptionHandler &getExceptionHandler() const = 0;

        /**
         * @brief Adds a new custom extension to MIME type mapping
         */
        virtual HttpAppFramework &registerCustomExtensionMime(
            const std::string &ext,
            const std::string &mime) = 0;

        virtual HttpAppFramework &enableCompressedRequest(bool enable = true) = 0;
        virtual bool isCompressedRequestEnabled() const = 0;
        /*
         * @brief get the number of active connections.
         */
        virtual int64_t getConnectionCount() const = 0;

        /**
         * @brief Set the before listen setsockopt callback.
         *
         * @param cb This callback will be called before the listen
         */
        virtual HttpAppFramework &setBeforeListenSockOptCallback(
            std::function<void(int)> cb) = 0;

        /**
         * @brief Set the after accept setsockopt callback.
         *
         * @param cb This callback will be called after accept
         */
        virtual HttpAppFramework &setAfterAcceptSockOptCallback(
            std::function<void(int)> cb) = 0;

        /**
         * @brief Set the client disconnect or connect callback.
         *
         * @param cb This callback will be called, when the client disconnect or
         * connect
         */
        virtual HttpAppFramework &setConnectionCallback(
            std::function<void(const trantor::TcpConnectionPtr &)> cb) = 0;

        virtual HttpAppFramework &enableRequestStream(bool enable = true) = 0;
        virtual bool isRequestStreamEnabled() const = 0;

    private:
        virtual void registerHttpController(
            const std::string &pathPattern,
            const internal::HttpBinderBasePtr &binder,
            const std::vector<HttpMethod> &validMethods = std::vector<HttpMethod>(),
            const std::vector<std::string> &filters = std::vector<std::string>(),
            const std::string &handlerName = "") = 0;
        virtual void registerHttpControllerViaRegex(
            const std::string &regExp,
            const internal::HttpBinderBasePtr &binder,
            const std::vector<HttpMethod> &validMethods,
            const std::vector<std::string> &filters,
            const std::string &handlerName) = 0;
    };

    inline HttpAppFramework &app()
    {
        return HttpAppFramework::instance();
    }

#ifdef __cpp_impl_coroutine
    namespace internal
    {
        inline void ForwardAwaiter::await_suspend(
            std::coroutine_handle<> handle) noexcept
        {
            app_.forward(
                req_,
                [this, handle](const xiaoHttp::HttpResponsePtr &resp)
                {
                    setValue(resp);
                    handle.resume();
                },
                host_,
                timeout_);
        }
    } // namespace internal
#endif
}