/**
 * @file HttpRequest.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-17
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <xiaoHttp/DrClassMap.h>
#include <xiaoHttp/HttpTypes.h>
#include <xiaoHttp/utils/Utilities.h>
#include <xiaoHttp/Session.h>
#include <xiaoHttp/Attribute.h>
#include <xiaoHttp/UploadFile.h>
#include <xiaoLog/Logger.h>
#include <xiaoLog/Date.h>
#include <xiaoNet/net/InetAddress.h>
#include <xiaoNet/net/Certificate.h>
#include <json/json.h>
#include <memory>

namespace xiaoHttp
{
    class HttpRequest;
    using HttpRequestPtr = std::shared_ptr<HttpRequest>;

    /**
     * @brief This template is used to convert a request object to a custom
     * type object. Users must specialize the template for a particular type.
     *
     * @tparam T
     * @return T
     */
    template <typename T>
    T fromRequest(const HttpRequest &)
    {
        LOG_ERROR << "You must specialize the fromRequest template for the type of "
                  << DrClassMap::demangle(typeid(T).name());
        exit(1);
    }

    /**
     * @brief This template is used to create a request object from a custom
     * type object by calling the newCustomHttpRequest(). Users must specialize
     * the template for a particular type.
     *
     * @tparam T
     * @return HttpRequestPtr
     */
    template <typename T>
    HttpRequestPtr toRequest(T &&)
    {
        LOG_ERROR << "You must specialize the toRequest template for the type of "
                  << DrClassMap::demangle(typeid(T).name());
        exit(1);
    }

    template <>
    HttpRequestPtr toRequest<const Json::Value &>(const Json::Value &pJson);
    template <>
    HttpRequestPtr toRequest(Json::Value &&pJson);

    template <>
    inline HttpRequestPtr toRequest<Json::Value &>(Json::Value &pJson)
    {
        return toRequest((const Json::Value &)pJson);
    }

    template <>
    std::shared_ptr<Json::Value> fromRequest(const HttpRequest &req);

    /**
     * @brief Abstract class for webapp developer to get or set the Http request;
     *
     */
    class XIAOHTTP_EXPORT HttpRequest
    {
    public:
        /**
         * @brief This template enables implicit type conversion. For using this
         * template, user must specialize the fromRequest template. For example a
         * shared_ptr<Json::Value> specialization version is available above, so
         * we can use the following code to get a json object:
         * @code
         * std::shared_ptr<Json::Value> jsonPtr = *requestPtr;
         * @endcode
         * With this template, user can use their favorite JSON library instead of
         * the default jsoncpp library or convert the request to an object of any
         * custom type.
         *
         * @tparam T
         * @return T
         */
        template <typename T>
        operator T() const
        {
            return fromRequest<T>(*this);
        }

        /**
         * @brief This template enables explicit type conversion, see the above
         * template.
         *
         * @tparam T
         * @return T
         */
        template <typename T>
        T as() const
        {
            return fromRequest<T>(*this);
        }

        /**
         * @brief Return the method string of the request, such as GET, POST ,etc.
         *
         * @return const char*
         */
        virtual const char *methodString() const = 0;

        const char *getMethodString() const
        {
            return methodString();
        }

        virtual HttpMethod method() const = 0;

        HttpMethod getMethod() const
        {
            return method();
        }

        /**
         * @brief Check if the method is or was HttpMethod::Head
         * @details Allows to known that an incoming request is a HEAD request, since
         *          drogon sets the method to HttpMethod::Get before calling the
         *          controller
         *
         * @return true
         * @return false
         */
        virtual bool isHead() const = 0;

        virtual const std::string &getHeader(std::string key) const = 0;

        virtual void addHeader(std::string field, const std::string &value) = 0;
        virtual void addHeader(std::string field, std::string &&value) = 0;

        virtual void removeHeader(std::string key) = 0;

        virtual const std::string &getCookie(const std::string &field) const = 0;

        /**
         * @brief Get all headers of the request
         *
         * @return const SafeStringMap<std::string>&
         */
        virtual const SafeStringMap<std::string> &headers() const = 0;

        const SafeStringMap<std::string> &getHeaders() const
        {
            return headers();
        }

        /**
         * @brief Get all cookies of the request
         *
         * @return const SafeStringMap<std::string>&
         */
        virtual const SafeStringMap<std::string> &cookies() const = 0;

        const SafeStringMap<std::string> &getCookies() const
        {
            return cookies();
        }

        /// Get the query string of the request.
        /**
         * The query string is the substring after the '?' in the URL string.
         */
        virtual const std::string &query() const = 0;

        /// Get the query string of the request.
        const std::string &getQuery() const
        {
            return query();
        }

        /**
         * @brief Get the content string of the request, which is the body part of the request.
         *
         * @return std::string_view
         */
        std::string_view body() const
        {
            return std::string_view(bodyData(), bodyLength());
        }

        std::string_view getBody() const
        {
            return body();
        }

        virtual const char *bodyData() const = 0;
        virtual size_t bodyLength() const = 0;

        virtual void setBody(const std::string &body) = 0;

        virtual void setBody(std::string &&body) = 0;

        virtual const std::string &path() const = 0;

        virtual const std::string &getOriginalPath() const = 0;

        const std::string &getPath() const
        {
            return path();
        }

        std::string_view getMatchedPathPattern() const
        {
            return matchedPathPattern();
        }

        std::string_view matchedPathPattern() const
        {
            return std::string_view(matchedPathPatternData(),
                                    matchedPathPatternLength());
        }

        virtual const std::vector<std::string> &getRoutingParameters() const = 0;

        virtual void setRoutingParameters(std::vector<std::string> &&params) = 0;

        virtual const char *matchedPathPatternData() const = 0;

        virtual size_t matchedPathPatternLength() const = 0;

        virtual const char *versionString() const = 0;

        const char *getVersionString() const
        {
            return versionString();
        }

        virtual Version version() const = 0;

        Version getVersion() const
        {
            return version();
        }

        virtual const SessionPtr &session() const = 0;

        const SessionPtr &getSession() const
        {
            return session();
        }

        virtual const AttributesPtr &attributes() const = 0;

        const AttributesPtr &getAttributes() const
        {
            return attributes();
        }

        virtual const SafeStringMap<std::string> &parameters() const = 0;

        const SafeStringMap<std::string> &getParameters() const
        {
            return parameters();
        }

        virtual const std::string &getParameter(const std::string &key) const = 0;

        /**
         * @brief Get the Optional Parameter object
         *
         * @tparam T
         * @param key
         * @return std::optional<T>
         */
        template <typename T>
        std::optional<T> getOptionalParameter(const std::string &key)
        {
            auto &params = getParameters();
            auto it = params.find(key);
            if (it != params.end())
            {
                try
                {
                    return std::optional<T>(
                        xiaoHttp::utils::fromString<T>(it->second));
                }
                catch (const std::exception &e)
                {
                    LOG_ERROR << e.what();
                    return std::optional<T>{};
                }
            }
            else
            {
                return std::optional<T>{};
            }
        }

        /// @brief Return the remote IP address and port
        /// @return
        virtual const xiaoNet::InetAddress &peerAddr() const = 0;

        const xiaoNet::InetAddress &getPeerAddr() const
        {
            return peerAddr();
        }

        virtual const xiaoNet::InetAddress &localAddr() const = 0;

        const xiaoNet::InetAddress &getLocalAddr() const
        {
            return localAddr();
        }

        virtual const xiaoLog::Date &creationDate() const = 0;

        const xiaoLog::Date &getCreationDate() const
        {
            return creationDate();
        }

        virtual const xiaoNet::CertificatePtr &peerCertificate() const = 0;

        const xiaoNet::CertificatePtr &getPeerCertificate() const
        {
            return peerCertificate();
        }

        virtual const std::shared_ptr<Json::Value> &jsonObject() const = 0;

        const std::shared_ptr<Json::Value> &getJsonObject() const
        {
            return jsonObject();
        }

        virtual const std::string &getJsonError() const = 0;

        virtual ContentType contentType() const = 0;

        ContentType getContentType() const
        {
            return contentType();
        }

        virtual void setMethod(const HttpMethod method) = 0;

        virtual void setPath(const std::string &path) = 0;
        virtual void setPath(std::string &&path) = 0;

        virtual void setPathEncode(bool) = 0;

        virtual void setParameter(const std::string &key,
                                  const std::string &value) = 0;

        virtual void setContentTypeCode(const ContentType type) = 0;

        void setContentTypeString(const std::string_view &typeString)
        {
            setContentTypeString(typeString.data(), typeString.size());
        }

        virtual void setCustomContentTypeString(const std::string &type) = 0;

        virtual void addCookie(const std::string &key,
                               const std::string &value) = 0;

        virtual void setPassThrough(bool flag) = 0;

        /// The following methods are a series of factory methods that help users
        /// create request objects.

        /// Create a normal request with http method Get and version Http1.1.
        static HttpRequestPtr newHttpRequst();

        static HttpRequestPtr newHttpJsonRequest(const Json::Value &data);

        static HttpRequestPtr newHttpForPostRequest();

        static HttpRequestPtr newFileUploadRequest(
            const std::vector<UploadFile> &files);

        template <typename T>
        static HttpRequestPtr newCustomHttpRequest(T &&obj)
        {
            return toRequest(std::forward<T>(obj));
        }

        virtual bool isOnSecureConnection() const noexcept = 0;
        virtual void setContentTypeString(const char *typeString,
                                          size_t typeStringLength) = 0;

        virtual ~HttpRequest()
        {
        }
    };

    template <>
    inline HttpRequestPtr toRequest<const Json::Value &>(const Json::Value &pJson)
    {
        return HttpRequest::newHttpJsonRequest(pJson);
    }

    template <>
    inline HttpRequestPtr toRequest(Json::Value &&pJson)
    {
        return HttpRequest::newHttpJsonRequest(std::move(pJson));
    }

    template <>
    inline std::shared_ptr<Json::Value> fromRequest(const HttpRequest &req)
    {
        return req.getJsonObject();
    }
}