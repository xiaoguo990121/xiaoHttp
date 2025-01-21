/**
 * @file HttpFileUploadRequest.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-21
 *
 *
 */

#pragma once
#include "HttpRequestImpl.h"
#include <string>
#include <vector>

namespace xiaoHttp
{
    class HttpFileUploadRequest : public HttpReqeustImpl
    {
    public:
        const std::string &boundary() const
        {
            return boundary_;
        }

        const std::vector<UploadFile> &files() const
        {
            return files_;
        }

        explicit HttpFileUploadRequest(const std::vector<UploadFile> &files);

    private:
        std::string boundary_;
        std::vector<UploadFile> files_;
    };
}