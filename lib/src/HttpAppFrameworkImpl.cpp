/**
 * @file HttpAppFrameworkImpl.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#include "HttpAppFrameworkImpl.h"
#include <xiaoHttp/HttpResponse.h>

#include "HttpResponseImpl.h"
#include "StaticFileRouter.h"

#include <iostream>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <filesystem>

#include <fcntl.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>
#define os_access access
#elif !defined(_WIN32) || defined(__MINGW32__)
#include <sys/file.h>
#include <unistd.h>
#define os_access access
#else
#include <io.h>
#define os_access _waccess
#define R_OK 04
#define W_OK 02
#endif

using namespace xiaoHttp;

HttpAppFrameworkImpl::HttpAppFrameworkImpl()
    : listenerManagerPtr_(new ListenerManager),
      pluginsManagerPtr_(new PluginsManager),
      uploadPath_(rootPath_ + "uploads")
{
}

static std::function<void()> f = []
{
    LOG_TRACE << "Initialize the main event loop int the main thread";
};

xiaoHttp::InitBeforeMainFunction xiaoHttp::HttpAppFrameworkImpl::initFirst_([]()
                                                                            { HttpAppFrameworkImpl::instance().getLoop()->runInLoop(f); });

namespace xiaoHttp
{
    std::string getVersion()
    {
        return XIAOHTTP_VERSION;
    }

    std::string getGitCommit()
    {
        return XIAOHTTP_VERSION_SHA1;
    }

    HttpResponsePtr defaultErrorHandler(HttpStatusCode code, const HttpResponsePtr &)
    {
        return std::make_shared<HttpResponseImpl>(code, CT_TEXT_HTML);
    }

    void defaultExceptionHandler(
        const std::exception &e,
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback)
    {
        std::string pathWithQuery = req->path();
        if (req->query().empty() == false)
            pathWithQuery += "?" + req->query();
        LOG_ERROR << "Unhandled exception in " << pathWithQuery
                  << ", what(): " << e.what();
        const auto &handler = app().getCustomErrorHandler();
        callback(handler(k500InternalServerError, req));
    }

    static void godaemon()
    {
        printf("Initializing daemon mode\n");
#ifndef _WIN32
        if (getppid() != 1)
        {
            pid_t pid;
            pid = fork();
            if (pid > 0)
                exit(0);
            if (pid < 0)
            {
                perror("fork");
                exit(1);
            }
            setsid();
        }

        close(0);
        close(1);
        close(2);

        int ret = open("/dev/null", O_RDWR);
        (void)ret;
        ret = dup(0);
        (void)ret;
        ret = dup(0);
        (void)ret;
        umask(0);
#else
        LOG_ERROR << "Cannot run as daemon in Windows";
        exit(1);
#endif

        return;
    }

    static void TERMFunction(int sig)
    {
        if (sig == SIGTERM)
        {
            LOG_WARN << "SIGTERM signal received.";
            HttpAppFramework::instance().getTermSignalHandler()();
        }
        else if (sig == SIGINT)
        {
            LOG_WARN << "SIGINT signal received.";
            HttpAppFrameworkImpl::instance().getIntSignalHandler()();
        }
    }
}

HttpAppFrameworkImpl::~HttpAppFrameworkImpl() noexcept
{
#ifndef _WIN32
    sharedLibManagerPtr_.reset();
#endif
    sessionManagerPtr_.reset();
}

HttpAppFramework &HttpAppFrameworkImpl::setStaticFilesCacheTime(int cacheTime)
{
    StaticFileRouter::instance().steStaticFileCacheTime(cacheTime);
    return *this;
}

int HttpAppFrameworkImpl::staticFilesCacheTime() const
{
    return StaticFileRouter::instance().staticFilesCacheTime();
}

HttpAppFramework &HttpAppFrameworkImpl::setGzipStatic(bool useGzipStatic)
{
    StaticFileRouter::instance().setGzipStatic(useGzipStatic);
    return *this;
}

HttpAppFramework &HttpAppFrameworkImpl::setBrStatic(bool useGzipStatic)
{
    StaticFileRouter::instance().setBrStatic(useGzipStatic);
    return *this;
}

HttpAppFramework &HttpAppFrameworkImpl::setImplicitPageEnable(
    bool useImplicitPage)
{
    StaticFileRouter::instance().setImplicitPageEnable(useImplicitPage);
    return *this;
}

bool HttpAppFrameworkImpl::isImplicitPageEnabled() const
{
    return StaticFileRouter::instance().isImplicitPageEnabled();
}

HttpAppFramework &HttpAppFrameworkImpl::setImplicitPage(
    const std::string &implicitPageFile)
{
    StaticFileRouter::instance().setImplicitPage(implicitPageFile);
    return *this;
}

const std::string &HttpAppFrameworkImpl::getImplicitPage() const
{
    return StaticFileRouter::instance().getImplicitPage();
}

#ifndef _WIN32
HttpAppFramework &HttpAppFrameworkImpl::enableDynamicViewsLoading(
    const std::vector<std::string> &libPaths,
    const std::string &outputPath)
{
    assert(!running_);

    for (auto const &libpath : libPaths)
    {
        if (libpath[0] == '/' ||
            (libpath.length() >= 2 && libpath[0] == '.' && libpath[1] == '/') ||
            (libpath.length() >= 3 && libpath[0] == '.' && libpath[1] == '.' &&
             libpath[2] == '/') ||
            libpath == "." || libpath == "..")
        {
            libFilePaths_.push_back(libpath);
        }
        else
        {
            if (rootPath_[rootPath_.length() - 1] == '/')
                libFilePaths_.push_back(rootPath_ + libpath);
            else
                libFilePaths_.push_back(rootPath_ + "/" + libpath);
        }
    }
    libFileOutputPath_ = outputPath;
    if (!libFileOutputPath_.empty())
    {
        if (xiaoHttp::utils::createPath(libFileOutputPath_) == -1)
        {
            LOG_FATAL << "Can't create " << libFileOutputPath_
                      << " path for dynamic views";
            exit(-1);
        }
    }

    return *this;
}
#endif
HttpAppFramework &HttpAppFrameworkImpl::setFileTypes(
    const std::vector<std::string> &types)
{
    StaticFileRouter::instance().setFileTypes(types);
    return *this;
}