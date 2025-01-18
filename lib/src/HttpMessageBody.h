/**
 * @file HttpMessageBody.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once
#include <string_view>
#include <string>
#include <memory>

namespace xiaoHttp
{
    class HttpMessageBody
    {
    public:
        enum class BodyType
        {
            kNone = 0,
            kString,
            kStringView
        };

        BodyType bodyType()
        {
            return type_;
        }

        virtual const char *data() const
        {
            return nullptr;
        }

        virtual char *data()
        {
            return nullptr;
        }

        virtual size_t length() const
        {
            return 0;
        }

        virtual std::string_view getString() const = 0;

        virtual void append(const char *, size_t)
        {
        }

        virtual ~HttpMessageBody()
        {
        }

    protected:
        BodyType type_{BodyType::kNone};
    };

    class HttpMessageStringBody : public HttpMessageBody
    {
    public:
        HttpMessageStringBody()
        {
            type_ = BodyType::kString;
        }

        HttpMessageStringBody(const std::string &body) : body_(body)
        {
            type_ = BodyType::kString;
        }

        HttpMessageStringBody(std::string &&body) : body_(std::move(body))
        {
            type_ = BodyType::kString;
        }

        const char *data() const override
        {
            return body_.data();
        }

        char *data() override
        {
            return const_cast<char *>(body_.data());
        }

        size_t length() const override
        {
            return body_.length();
        }

        std::string_view getString() const override
        {
            return std::string_view{body_.data(), body_.length()};
        }

        void append(const char *buf, size_t len) override
        {
            body_.append(buf, len);
        }

    private:
        std::string body_;
    };

    class HttpMessageStringViewBody : public HttpMessageBody
    {
    public:
        HttpMessageStringViewBody(const char *buf, size_t len) : body_(buf, len)
        {
            type_ = BodyType::kStringView;
        }

        const char *data() const override
        {
            return body_.data();
        }

        char *data() override
        {
            return const_cast<char *>(body_.data());
        }

        size_t length() const override
        {
            return body_.length();
        }

        std::string_view getString() const override
        {
            return body_;
        }

    private:
        std::string_view body_;
    };
}