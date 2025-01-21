/**
 * @file HttpFileUploadRequest.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-21
 *
 *
 */

#include "HttpFileUploadRequest.h"
#include <xiaoHttp/UploadFile.h>
#include <xiaoHttp/utils/Utilities.h>

using namespace xiaoHttp;

HttpFileUploadRequest::HttpFileUploadRequest(
    const std::vector<UploadFile> &files) : HttpRequestImpl(nullptr),
                                            boundary_(utils::genRandomString(32)),
                                            files_(files)
{
    setMethod(xiaoHttp::Post);
    setVersion(xiaoHttp::Version::kHttp11);
    setContentType("multipart/form-data; boundary=" + boundary_);
    contentType_ = CT_MULTIPART_FORM_DATA;
}