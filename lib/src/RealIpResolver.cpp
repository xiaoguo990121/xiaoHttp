/**
 * @file RealIpResolver.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-19
 *
 *
 */

#include <xiaoHttp/xiaoHttp.h>
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
    catch (const std::exception &ex)
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
        std::string ipOrCidr = elem.asString();
        trustCIDRs_.emplace_back(ipOrCidr);
    }

    xiaoHttp::app().registerPreRoutingAdvice([this](const HttpRequestPtr &req)
                                             {
                                                 const std::string &ipHeader = req->getHeader(fromHeader_);
                                                 const xiaoNet::InetAddress &peerAddr = req->getPeerAddr();
                                                 if (ipHeader.empty() || !matchCidr(peerAddr))
                                                 {
                                                     req->attributes()->insert(attributeKey_, peerAddr);
                                                     return;
                                                 }
                                                 if (!useXForwardedFor_)
                                                 {
                                                     xiaoNet::InetAddress addr = parseAddress(ipHeader);
                                                     if (addr.isUnspecified())
                                                     {
                                                         req->attributes()->insert(attributeKey_, peerAddr);
                                                     }
                                                     else
                                                     {
                                                         req->attributes()->insert(attributeKey_, addr);
                                                     }
                                                     return;
                                                 }

                                                 XForwardedForParser parser(ipHeader);
                                                 std::string ip;
                                                 while (!(ip = parser.getNext()).empty())
                                                 {
                                                     xiaoNet::InetAddress addr = parseAddress(ip);
                                                     if(addr.isUnspecified() || matchCidr(addr))
                                                     {
                                                        continue;
                                                     }
                                                     req->attributes()->insert(attributeKey_, addr);
                                                     return;
                                                 }
                                                 req->attributes()->insert(attributeKey_, peerAddr); });
}

void RealIpResolver::shutdown()
{
}

const xiaoNet::InetAddress &RealIpResolver::GetRealAddr(
    const HttpRequestPtr &req)
{
    auto *plugin = app().getPlugin<xiaoHttp::plugin::RealIpResolver>();
    if (!plugin)
    {
        return req->getPeerAddr();
    }
    return plugin->getRealAddr(req);
}

const xiaoNet::InetAddress &RealIpResolver::getRealAddr(
    const HttpRequestPtr &req) const
{
    const std::shared_ptr<Attributes> &attributesPtr = req->getAttributes();
    if (!attributesPtr->find(attributeKey_))
    {
        return req->getPeerAddr();
    }
    return attributesPtr->get<xiaoNet::InetAddress>(attributeKey_);
}

bool RealIpResolver::matchCidr(const xiaoNet::InetAddress &addr) const
{
    for (auto &cidr : trustCIDRs_)
    {
        if ((addr.ipNetEndian() & cidr.mask_) == cidr.addr_)
        {
            return true;
        }
    }
    return false;
}

RealIpResolver::CIDR::CIDR(const std::string &ipOrCidr)
{
    // Find CIDR slash
    auto pos = ipOrCidr.find('/');
    std::string ipv4;
    if (pos != std::string::npos)
    {
        // parameter is a CIDR block
        std::string prefixLen = ipOrCidr.substr(pos + 1);
        ipv4 = ipOrCidr.substr(0, pos);
        uint16_t prefix = std::stoi(prefixLen);
        if (prefix > 32)
        {
            throw std::runtime_error("Bad CIDR block: " + ipOrCidr);
        }
        mask_ = htonl(0xffffffffu << (32 - prefix));
    }
    else
    {
        // parameter is an IP
        ipv4 = ipOrCidr;
        mask_ = 0xffffffffu;
    }

    xiaoNet::InetAddress addr(ipv4, 0);
    if (addr.isIpV6())
    {
        throw std::runtime_error("Ipv6 is not supported by RealIpResolver.");
    }
    if (addr.isUnspecified())
    {
        throw std::runtime_error("Bad ipv4 address: " + ipv4);
    }
    addr_ = addr.ipNetEndian() & mask_;
}