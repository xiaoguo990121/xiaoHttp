/**
 * @file HttpViewData.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#include <xiaoHttp/HttpViewData.h>

using namespace xiaoHttp;

std::string HttpViewData::htmlTranslate(const char *str, size_t length)
{
    std::string ret;
    ret.reserve(length + 64);
    auto end = str + length;
    while (str != end)
    {
        switch (*str)
        {
        case '"':
            ret.append("&quot;", 6);
            break;
        case '&':
            ret.append("&amp;", 5);
            break;
        case '<':
            ret.append("&lt;", 4);
            break;
        case '>':
            ret.append("&gt;", 4);
            break;
        default:
            ret.push_back(*str);
            break;
        }
        ++str;
    }
    return ret;
}