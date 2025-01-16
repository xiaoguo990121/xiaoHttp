/**
 * @file HttpUtils.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 *
 */

#pragma once

#include <xiaoNet/utils/MsgBuffer.h>
#include <xiaoHttp/HttpTypes.h>
#include <string>

namespace xiaoHttp
{
    const std::string_view &contentTypeToMime(ContentType contentType);
    const std::string_view &statusCodeToString(int code);
}