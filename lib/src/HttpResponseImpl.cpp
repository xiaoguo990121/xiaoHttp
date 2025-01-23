/**
 * @file HttpResponseImpl.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-23
 *
 *
 */

#include "HttpResponseImpl.h"
#include "AOPAdvice.h"
#include "HttpAppFrameworkImpl.h"
#include "HttpUtils.h"
#include <xiaoHttp/HttpViewData.h>

using namespace xiaoHttp;
using namespace xiaoNet;
using namespace xiaoLog;

namespace xiaoHttp
{
    // "Fri, 23 Aug 2019 12:58:03 GMT" length = 29
    static const size_t httpFullDateStringLength = 29;

    static inline HttpResponsePtr genHttpResponse(const std::string &viewName,
                                                  const HttpViewData &data,
                                                  const HttpRequestPtr &req)
    {
        auto templ = DrTemplateBase::newTemplate(viewName);
        if (templ)
        {
            auto res = HttpResponse::newHttpResponse();
            res->setBody(templ->genText(data));
            return res;
        }
        return xiaoHttp::HttpResponse::newNotFoundResponse(req);
    }
}

HttpResponsePtr HttpResponse::newHttpResponse()
{
    auto res = std::make_shared<HttpResponseImpl>(k200OK, CT_TEXT_HTML);
    AopAdvice::instance().passResponseCreationAdvices(res);
    return res;
}

HttpResponsePtr HttpResponse::newHttpResponse(HttpStatusCode code,
                                              ContentType type)
{
    auto res = std::make_shared<HttpResponseImpl>(code, type);
    AopAdvice::instance().passResponseCreationAdvices(res);
    return res;
}

HttpResponsePtr HttpRESPONSE::newHttpJsonResponse(const Json::Value &data)
{
    auto res = std::make_shared<HttpResponseImpl>(k200OK, CT_APPLICATION_JSON);
    res->setJsonObject(data);
    AopAdvice::instance().passResponseCreationAdvices(res);
    return res;
}

HttpResponsePtr HttpResponse::newHttpJsonResponse(Json::Value &&data)
{
    auto res = std::make_shared<HttpResponseImpl>(k200OK, CT_APPLICATION_JSON);
    res->setJsonObject(std::move(data));
    AopAdvice::instance().passResponseCreationAdvices(res);
    return res;
}

const char *HttpResponseImpl::versionString() const
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

void HttpResponseImpl::addHeader(const char *start,
                                 const char *colon,
                                 const char *end)
{
    fullHeaderString_.reset();
    std::string field(start, colon);
    transform(field.begin(), field.end(), field.begin(), [](unsigned char c)
              { return tolower(c); });
    ++colon;
    while (colon < end && isspace(static_cast<unsigned char>(*colon)))
    {
        ++colon;
    }
    std::string value(colon, end);
    while (!value.empty() &&
           isspace(static_cast<unsigned char>(value[value.size() - 1])))
    {
        value.resize(value.size() - 1);
    }

    if (field == "set-cookie")
    {
        // LOG_INFO<<"cookies!!!:"<<value;
        auto values = utils::splitString(value, ";");
        Cookie cookie;
        cookie.setHttpOnly(false);
        for (size_t i = 0; i < values.size(); ++i)
        {
            std::string &coo = values[i];
            std::string cookie_name;
            std::string cookie_value;
            auto epos = coo.find('=');
            if (epos != std::string::npos)
            {
                cookie_name = coo.substr(0, epos);
                std::string::size_type cpos = 0;
                while (cpos < cookie_name.length() &&
                       isspace(static_cast<unsigned char>(cookie_name[cpos])))
                    ++cpos;
                cookie_name = cookie_name.substr(cpos);
                ++epos;
                while (epos < coo.length() &&
                       isspace(static_cast<unsigned char>(coo[epos])))
                    ++epos;
                cookie_value = coo.substr(epos);
            }
            else
            {
                std::string::size_type cpos = 0;
                while (cpos < coo.length() &&
                       isspace(static_cast<unsigned char>(coo[cpos])))
                    ++cpos;
                cookie_name = coo.substr(cpos);
            }
            if (i == 0)
            {
                cookie.setKey(cookie_name);
                cookie.setValue(cookie_value);
            }
            else
            {
                std::transform(cookie_name.begin(),
                               cookie_name.end(),
                               cookie_name.begin(),
                               [](unsigned char c)
                               { return tolower(c); });
                if (cookie_name == "path")
                {
                    cookie.setPath(cookie_value);
                }
                else if (cookie_name == "domain")
                {
                    cookie.setDomain(cookie_value);
                }
                else if (cookie_name == "expires")
                {
                    cookie.setExpiresDate(utils::getHttpDate(cookie_value));
                }
                else if (cookie_name == "secure")
                {
                    cookie.setSecure(true);
                }
                else if (cookie_name == "httponly")
                {
                    cookie.setHttpOnly(true);
                }
                else if (cookie_name == "samesite")
                {
                    cookie.setSameSite(
                        cookie.convertString2SameSite(cookie_value));
                }
                else if (cookie_name == "max-age")
                {
                    cookie.setMaxAge(std::stoi(cookie_value));
                }
            }
        }
        if (!cookie.key().empty())
        {
            cookies_[cookie.key()] = std::move(cookie);
        }
    }
    else
    {
        headers_[std::move(field)] = std::move(value);
    }
}

void HttpResponseImpl::renderToBuffer(xiaoNet::MsgBuffer &buffer)
{
    if (expriedTime_ >= 0)
    {
        auto strPtr = renderToBuffer();
        buffer.append(strPtr->peek(), strPtr->readableBytes());
        return;
    }

    if (!fullHeaderString_)
    {
        makeHeaderString(buffer);
    }
    else
    {
        buffer.append(*fullHeaderString_);
    }

    // output cookies
    if (!cookies_.empty())
    {
        for (auto it = cookies_.begin(); it != cookies_.end(); ++it)
        {
            buffer.append(it->second.cookieString());
        }
    }

    // output Date header
    if (!passThrough_ &&
        drogon::HttpAppFrameworkImpl::instance().sendDateHeader())
    {
        buffer.append("date: ");
        buffer.append(utils::getHttpFullDateStr(xiaoLog::Date::date()));
        buffer.append("\r\n\r\n");
    }
    else
    {
        buffer.append("\r\n");
    }
    if (bodyPtr_ && contentLengthIsAllowed())
        buffer.append(bodyPtr_->data(), bodyPtr_->length());
}

std::shared_ptr<xiaoNet::MsgBuffer> HttpResponseImpl::renderToBuffer()
{
    if (expriedTime_ >= 0)
    {
        if (!passThrough_ &&
            xiaoHttp::HttpAppFrameworkImpl::instance().sendDateHeader())
        {
            if (datePos_ != static_cast<size_t>(-1))
            {
                auto now = xiaoLog::Date::now();
                bool isDateChanged =
                    ((now.microSecondsSinceEpoch() / MICRO_SECONDS_PRE_SEC) !=
                     httpStringDate_);
                assert(httpString_);
                if (isDateChanged)
                {
                    httpStringDate_ =
                        now.microSecondsSinceEpoch() / MICRO_SECONDS_PRE_SEC;
                    auto newDate = utils::getHttpFullDate(now);

                    httpString_ =
                        std::make_shared<xiaoNet::MsgBuffer>(*httpString_);
                    memcpy((void *)&(*httpString_)[datePos_],
                           newDate,
                           httpFullDateStringLength);
                    return httpString_;
                }

                return httpString_;
            }
        }
        else
        {
            if (httpString_)
                return httpString_;
        }
    }
    auto httpString = std::make_shared<xiaoNet::MsgBuffer>(256);
    if (!fullHeaderString_)
    {
        makeHeaderString(*httpString);
    }
    else
    {
        httpString->append(*fullHeaderString_);
    }

    // output cookies
    if (!cookies_.empty())
    {
        for (auto it = cookies_.begin(); it != cookies_.end(); ++it)
        {
            httpString->append(it->second.cookieString());
        }
    }

    // output Date header
    if (!passThrough_ &&
        xiaoHttp::HttpAppFrameworkImpl::instance().sendDateHeader())
    {
        httpString->append("date: ");
        auto datePos = httpString->readableBytes();
        httpString->append(utils::getHttpFullDateStr(xiaoLog::Date::date()));
        httpString->append("\r\n\r\n");
        datePos_ = datePos;
    }
    else
    {
        httpString->append("\r\n");
    }

    LOG_TRACE << "response(no body):"
              << std::string_view{httpString->peek(),
                                  httpString->readableBytes()};
    if (bodyPtr_)
        httpString->append(bodyPtr_->data(), bodyPtr_->length());
    if (expriedTime_ >= 0)
    {
        httpString_ = httpString;
    }
    return httpString;
}

std::shared_ptr<xiaoNet::MsgBuffer> HttpResponseImpl::
    renderHeaderForHeadMethod()
{
    auto httpString = std::make_shared<xiaoNet::MsgBuffer>(256);
    if (!fullHeaderString_)
    {
        makeHeaderString(*httpString);
    }
    else
    {
        httpString->append(*fullHeaderString_);
    }

    // output cookies
    if (!cookies_.empty())
    {
        for (auto it = cookies_.begin(); it != cookies_.end(); ++it)
        {
            httpString->append(it->second.cookieString());
        }
    }

    // output Date header
    if (!passThrough_ &&
        xiaoHttp::HttpAppFrameworkImpl::instance().sendDateHeader())
    {
        httpString->append("date: ");
        httpString->append(utils::getHttpFullDate(xiaoLog::Date::date()),
                           httpFullDateStringLength);
        httpString->append("\r\n\r\n");
    }
    else
    {
        httpString->append("\r\n");
    }

    return httpString;
}

void HttpResponseImpl::clear()
{
    statusCode_ = kUnknown;
    version_ = Version::kHttp11;
    statusMessage_ = std::string_view{};
    fullHeaderString_.reset();
    jsonParsingErrorPtr_.reset();
    sendfileName_.clear();
    if (streamCallback_)
    {
        LOG_TRACE << "Cleanup HttpResponse stream callback";
        streamCallback_(nullptr, 0); // callback internal cleanup
        streamCallback_ = {};
    }
    if (asyncStreamCallback_)
    {
        // asyncStreamCallback_(nullptr);
        asyncStreamCallback_ = {};
    }
    headers_.clear();
    cookies_.clear();
    bodyPtr_.reset();
    jsonPtr_.reset();
    expriedTime_ = -1;
    datePos_ = std::string::npos;
    flagForParsingContentType_ = false;
    flagForParsingJson_ = false;
}

void HttpResponseImpl::parseJson() const
{
    static std::once_flag once;
    static Json::CharReaderBuilder builder;
    std::call_once(once, []()
                   {
        builder["collectComments"] = false;
        builder["stackLimit"] =
            static_cast<Json::UInt>(drogon::app().getJsonParserStackLimit()); });
    JSONCPP_STRING errs;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (bodyPtr_)
    {
        jsonPtr_ = std::make_shared<Json::Value>();
        if (!reader->parse(bodyPtr_->data(),
                           bodyPtr_->data() + bodyPtr_->length(),
                           jsonPtr_.get(),
                           &errs))
        {
            LOG_ERROR << errs;
            LOG_ERROR << "body: " << bodyPtr_->getString();
            jsonPtr_.reset();
            jsonParsingErrorPtr_ =
                std::make_shared<std::string>(std::move(errs));
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
            std::make_shared<std::string>("empty response body");
    }
}

bool HttpResponseImpl::shouldBeCompressed() const
{
    if (streamCallback_ || asyncStreamCallback_ || !sendfileName_.empty() ||
        contentType() >= CT_APPLICATION_OCTET_STREAM ||
        getBody().length() < 1024 ||
        !(getHeaderBy("content-encoding").empty()) || !contentLengthIsAllowed())
    {
        return false;
    }
    return true;
}

void HttpResponseImpl::setContentTypeString(const char *typeString,
                                            size_t typeStringLength)
{
    std::string sv(typeString, typeStringLength);
    auto contentType = parseContentType(sv);
    if (contentType == CT_NONE)
        contentType = CT_CUSTOM;
    contentType_ = contentType;
    contentTypeString_ = std::string(sv);
    flagForParsingContentType_ = true;
}

void HttpResponseImpl::swap(HttpResponseImpl &that) noexcept
{
    using std::swap;
    headers_.swap(that.headers_);
    cookies_.swap(that.cookies_);
    swap(statusCode_, that.statusCode_);
    swap(version_, that.version_);
    swap(statusMessage_, that.statusMessage_);
    swap(closeConnection_, that.closeConnection_);
    bodyPtr_.swap(that.bodyPtr_);
    swap(contentType_, that.contentType_);
    swap(flagForParsingContentType_, that.flagForParsingContentType_);
    swap(flagForParsingJson_, that.flagForParsingJson_);
    swap(sendfileName_, that.sendfileName_);
    swap(streamCallback_, that.streamCallback_);
    swap(asyncStreamCallback_, that.asyncStreamCallback_);
    jsonPtr_.swap(that.jsonPtr_);
    fullHeaderString_.swap(that.fullHeaderString_);
    httpString_.swap(that.httpString_);
    swap(datePos_, that.datePos_);
    swap(jsonParsingErrorPtr_, that.jsonParsingErrorPtr_);
}

void HttpResponseImpl::generateBodyFromJson() const
{
    if (!jsonPtr_ || flagForSerializingJson_)
    {
        return;
    }
    flagForSerializingJson_ = true;
    static std::once_flag once;
    static Json::StreamWriterBuilder builder;
    std::call_once(once, []()
                   {
        builder["commentStyle"] = "None";
        builder["indentation"] = "";
        if (!app().isUnicodeEscapingUsedInJson())
        {
            builder["emitUTF8"] = true;
        }
        auto &precision = app().getFloatPrecisionInJson();
        if (precision.first != 0)
        {
            builder["precision"] = precision.first;
            builder["precisionType"] = precision.second;
        } });
    bodyPtr_ = std::make_shared<HttpMessageStringBody>(
        writeString(builder, *jsonPtr_));
}

void HttpResponseImpl::makeHeaderString(xiaoNet::MsgBuffer &buffer)
{
    buffer.ensureWritableBytes(128);
    int len{0};
    if (version_ == Version::kHttp11)
    {
        if (customStatusCode_ >= 0)
        {
            len = snprintf(buffer.beginWrite(),
                           buffer.writableBytes(),
                           "HTTP/1.1 %d ",
                           customStatusCode_);
        }
        else
        {
            len = snprintf(buffer.beginWrite(),
                           buffer.writableBytes(),
                           "HTTP/1.1 %d ",
                           statusCode_);
        }
    }
    else
    {
        if (customStatusCode_ >= 0)
        {
            len = snprintf(buffer.beginWrite(),
                           buffer.writableBytes(),
                           "HTTP/1.0 %d ",
                           customStatusCode_);
        }
        else
        {
            len = snprintf(buffer.beginWrite(),
                           buffer.writableBytes(),
                           "HTTP/1.0 %d ",
                           statusCode_);
        }
    }

    buffer.hasWritten(len);

    if (!statusMessage_.empty())
        buffer.append(statusMessage_.data(), statusMessage_.length());
    buffer.append("\r\n");
    generateBodyFromJson();
    if (!passThrough_)
    {
        buffer.ensureWritableBytes(64);
        if (!contentLengthIsAllowed())
        {
            len = 0;
            if ((bodyPtr_ && bodyPtr_->length() > 0) ||
                !sendfileName_.empty() || streamCallback_ ||
                asyncStreamCallback_)
            {
                LOG_ERROR << "The body should be empty when the content-length "
                             "is not allowed!";
            }
        }
        else if (streamCallback_ || asyncStreamCallback_)
        {
            // When the headers are created, it is time to set the transfer
            // encoding to chunked if the contents size is not specified
            if (!ifCloseConnection() &&
                headers_.find("content-length") == headers_.end())
            {
                LOG_DEBUG << "send stream with transfer-encoding chunked";
                headers_["transfer-encoding"] = "chunked";
            }
            len = 0;
        }
        else if (sendfileName_.empty())
        {
            auto bodyLength = bodyPtr_ ? bodyPtr_->length() : 0;
            len = snprintf(buffer.beginWrite(),
                           buffer.writableBytes(),
                           contentLengthFormatString<decltype(bodyLength)>(),
                           bodyLength);
        }
        else
        {
            auto bodyLength = sendfileRange_.second;
            len = snprintf(buffer.beginWrite(),
                           buffer.writableBytes(),
                           contentLengthFormatString<decltype(bodyLength)>(),
                           bodyLength);
        }
        buffer.hasWritten(len);
        if (headers_.find("connection") == headers_.end())
        {
            if (closeConnection_)
            {
                buffer.append("connection: close\r\n");
            }
            else if (version_ == Version::kHttp10)
            {
                buffer.append("connection: Keep-Alive\r\n");
            }
        }

        if (!contentTypeString_.empty())
        {
            buffer.append("content-type: ");
            buffer.append(contentTypeString_);
            buffer.append("\r\n");
        }
        if (HttpAppFrameworkImpl::instance().sendServerHeader())
        {
            buffer.append(
                HttpAppFrameworkImpl::instance().getServerHeaderString());
        }
    }

    for (auto it = headers_.begin(); it != headers_.end(); ++it)
    {
        buffer.append(it->first);
        buffer.append(": ");
        buffer.append(it->second);
        buffer.append("\r\n");
    }
}