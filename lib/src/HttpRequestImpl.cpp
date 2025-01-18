/**
 * @file HttpRequestImpl.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-17
 *
 *
 */

#include "HttpRequestImpl.h"
#include "HttpAppFrameworkImpl.h"

using namespace xiaoHttp;

void HttpRequestImpl::parseJson() const
{
    auto input = contentView();
    if (input.empty())
        return;
    if (contentType_ == CT_APPLICATION_JSON ||
        getHeaderBy("content-type").find("application/json") !=
            std::string::npos)
    {
        static std::once_flag once;
        static Json::CharReaderBuilder builder;
        std::call_once(once, []()
                       { builder["collectComments"] = false;
                       builder["stackLimit"] = static_cast<Json::UInt>(
                        xiaoHttp::app().getJsonParseStackLimit()
                       ); });
    }
}