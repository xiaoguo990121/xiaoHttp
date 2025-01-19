/**
 * @file StaticFileRouter.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#pragma once

#include "impl_forwards.h"
#include "FiltersFunction.h"
#include <xiaoHttp/CacheMap.h>
#include <xiaoHttp/IOThreadStorage.h>

#include <functional>
#include <set>
#include <string>
#include <memory>

namespace xiaoHttp
{
    class StaticFileRouter
    {

    public:
        static StaticFileRouter &instance()
        {
            static StaticFileRouter inst;
            return inst;
        }

        void route(const HttpRequestImplPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
        void setFileTypes(const std::vector<std::string> &types);

        void setStaticFilesCacheTime(int cacheTime)
        {
            staticFilesCacheTime_ = cacheTime;
        }

        int staticFilesCacheTime() const
        {
            return staticFilesCacheTime_;
        }

        void setGzipStatic(bool useGzipStatic)
        {
            gzipStaticFlag_ = useGzipStatic;
        }

        void setBrStatic(bool useBrStatic)
        {
            brStaticFlag_ = useBrStatic;
        }

        void init(const std::vector<xiaoNet::EventLoop *> &ioLoops);
        void reset();

        void sendStaticFileResponse(
            const std::string &filePath,
            const HttpRequestImplPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback,
            const std::string_view &defaultContentType);

        void addALocation(const std::string &uriPrefix,
                          const std::string &defaultContentType,
                          const std::string &alias,
                          bool isCaseSensitive,
                          bool allowAll,
                          bool isRecursive,
                          const std::vector<std::string> &filters)
        {
            locations_.emplace_back(uriPrefix,
                                    defaultContentType,
                                    alias,
                                    isCaseSensitive,
                                    allowAll,
                                    isRecursive,
                                    filters);
        }

        void setStaticFileHeaders(
            const std::vector<std::pair<std::string, std::string>> &headers)
        {
            headers_ = headers;
        }

        void setImplicitPageEnable(bool useImplicitPage)
        {
            implicitPageEnable_ = useImplicitPage;
        }

        bool isImplicitPageEnabled() const
        {
            return implicitPageEnable_;
        }

        void setImplicitPage(const std::string &implicitPageFile)
        {
            implicitPage_ = implicitPageFile;
        }

        const std::string &getImplicitPage() const
        {
            return implicitPage_;
        }

        void setDefaultHandler(DefaultHandler &&handler)
        {
            defaultHandler_ = std::move(handler);
        }

    private:
        static void defaultHandler(
            const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback);

        std::set<std::string> fileTypeSet_{"html",
                                           "js",
                                           "css",
                                           "xml",
                                           "xsl",
                                           "txt",
                                           "svg",
                                           "ttf",
                                           "otf",
                                           "woff2",
                                           "eot",
                                           "png",
                                           "jpg",
                                           "jpeg",
                                           "gif",
                                           "bmp",
                                           "ico",
                                           "icns"};

        int staticFilesCacheTime_{5};
        bool enableLastModify_{true};
        bool enableRange_{true};
        bool gzipStaticFlag_{true};
        bool brStaticFlag_{true};
        std::unique_ptr<
            IOThreadStorage<std::unique_ptr<CacheMap<std::string, char>>>>
            staticFilesCacheMap_;
        std::unique_ptr<
            IOThreadStorage<std::unordered_map<std::string, HttpRequestPtr>>>
            staticFilesCache_;
        std::vector<std::pair<std::string, std::string>> headers_;
        bool implicitPageEnable_{true};
        std::string implicitPage_{"index.html"};
        DefaultHandler defaultHandler_ = StaticFileRouter::defaultHandler;

        struct Location
        {
            std::string uriPrefix_;
            std::string defaultContentType_;
            std::string alias_;
            std::string realLocation_;
            bool isCaseSensitive_;
            bool allowAll_;
            bool isRecursive_;
            std::vector<std::shared_ptr<xiaoHttp::HttpFilterBase>> filters_;

            Location(const std::string &uriPrefix,
                     const std::string &defaultContentType,
                     const std::string &alias,
                     bool isCaseSensitive,
                     bool allowAll,
                     bool isRecursive,
                     const std::vector<std::string> &filters)
                : uriPrefix_(uriPrefix),
                  alias_(alias),
                  isCaseSensitive_(isCaseSensitive),
                  allowAll_(allowAll),
                  isRecursive_(isRecursive),
                  filters_(filters_function::createFilters(filters))
            {
                if (!defaultContentType.empty())
                {
                    defaultContentType_ =
                        std::string{"content-type: "} + defaultContentType + "\r\n";
                }
            }
        };

        std::shared_ptr<IOThreadStorage<std::vector<Location>>> ioLocationsPtr_;
        std::vector<Location> locations_;
    };
}