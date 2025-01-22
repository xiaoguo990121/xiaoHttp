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
        jsonPtr_ = std::make_shared<Json::Value>();
        JSONCPP_STRING errs;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (!reader->parse(input.data(),
                           input.data() + input.size(),
                           jsonPtr_.get(),
                           &errs))
        {
            LOG_DEBUG << errs;
            jsonPtr_.reset();
            jsonParsingErrorPtr_ =
                std::make_unique<std::string>(std::move(errs));
        }
        else
        {
            jsonParsingErrorPtr_.reset();
        }
    }
    else
    {
        jsonPtr_.reset();
        jsonParsingErrorPtr_ =
            std::make_unique<std::string>("content type error");
    }
}

const char *HttpRequestImpl::versionString() const
{
    const char *result = "UNKNOWN";
    switch (version_)
    {
    case Version::kHttp10:
        result = "HTTP/1.0";
        break;

    case Version::kHttp11:
        result = "HTTP/1.1";
        break;

    default:
        break;
    }
    return result;
}

bool HttpRequestImpl::setMethod(const char *start, const char *end)
{
    assert(method_ == Invalid);
    std::string_view m(start, end - start);
    switch (m.length())
    {
    case 3:
        if (m == "GET")
        {
            method_ = Get;
        }
        else if (m == "PUT")
        {
            method_ = Put;
        }
        else
        {
            method_ = Invalid;
        }
        break;
    case 4:
        if (m == "POST")
        {
            method_ = Post;
        }
        else if (m == "HEAD")
        {
            method_ = Head;
        }
        else
        {
            method_ = Invalid;
        }
        break;
    case 5:
        if (m == "PATCH")
        {
            method_ = Patch;
        }
        else
        {
            method_ = Invalid;
        }
        break;
    case 6:
        if (m == "DELETE")
        {
            method_ = Delete;
        }
        else
        {
            method_ = Invalid;
        }
        break;
    case 7:
        if (m == "OPTIONS")
        {
            method_ = Options;
        }
        else
        {
            method_ = Invalid;
        }
        break;
    default:
        method_ = Invalid;
        break;
    }

    return method_ != Invalid;
}