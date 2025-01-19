/**
 * @file RealIpResolver.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#include <xiaoHttp/plugins/RealIpResolver.h>
#include <xiaoLog/Logger.h>

using namespace xiaoHttp;
using namespace xiaoHttp::plugin;

struct XForwardedForParser : public xiaoNet::NonCopyable
{
    explicit XForwardedForParser(std::string value)
        : value_(std::move(value)), start_(value_.c_str()), len_(value_.size())
    {
    }

    std::string getNext()
    {
        if (len_ == 0)
        {
            return {};
        }
        // Skip trailing separators
        const char *cur;
        for (cur = start_ + len_ - 1; cur > start_; --cur, --len_)
        {
            if (*cur != ' ' && *cur != ',')
            {
                break;
            }
        }
        for (; cur > start_; --cur)
        {
            if (*cur == ' ' || *cur == ',')
            {
                ++cur;
                break;
            }
        }
        std::string ip{cur, len_ - (cur - start_)};
        len_ = cur == start_ ? 0 : cur - start_ - 1;
        return ip;
    }

private:
    std::string value_;
    const char *start_;
    size_t len_;
};

static xiaoNet::InetAddress parseAddress(const std::string &addr)
{
    auto pos = addr.find(':');
    uint16_t port = 0;
    if (pos == std::string::npos)
    {
        return xiaoNet::InetAddress(addr, 0);
    }
    try
    {
        port = std::stoi(addr.substr(pos + 1));
    }
    catch (const std::exception &e)
    {
        (void)ex;
        LOG_ERROR << "Error in ipv4 address: " + addr;
        port = 0;
    }
    return xiaoNet::InetAddress(addr.substr(0, pos), port);
}

void RealIpResolver::initAndStart(const Json::Value &config)
{
    fromHeader_ = config.get("from_header", "x-forwarded-for").asString();
    attributeKey_ = config.get("attribute_key", "real-ip").asString();

    std::transform(fromHeader_.begin(),
                   fromHeader_.end(),
                   fromHeader_.begin(),
                   [](unsigned char c)
                   { return tolower(c); });
    if (fromHeader_ == "x-forwarded-for")
    {
        useXForwardedFor_ = true;
    }

    const Json::Value &trustIps = config["trust_ips"];
    if (!trustIps.isArray())
    {
        throw std::runtime_error("Invalid trusted_ips. Should be array.");
    }
    for (const auto &elem : trustIps)
    {
    }
}