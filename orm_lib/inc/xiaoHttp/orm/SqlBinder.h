/**
 * @file SqlBinder.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 *
 */

#pragma once
#include <xiaoHttp/exports.h>
#include <xiaoHttp/orm/DbTypes.h>
#include <xiaoHttp/orm/Exception.h>
#include <xiaoHttp/orm/Field.h>
#include <xiaoHttp/orm/FunctionTraits.h>
#include <xiaoHttp/orm/ResultIterator.h>
#include <xiaoHttp/orm/Row.h>
#include <xiaoHttp/orm/RowIterator.h>
#include <string_view>
#include <json/writer.h>
#include <trantor/utils/Logger.h>
#include <trantor/utils/NonCopyable.h>
#include <json/json.h>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string.h>
#include <string>
#include <vector>
#include <optional>
#ifdef _WIN32
#include <winsock2.h>
#else // some Unix-like OS
#include <arpa/inet.h>
#endif

#if defined __linux__ || defined __FreeBSD__ || defined __OpenBSD__ || \
    defined __MINGW32__ || defined __HAIKU__

#ifdef __linux__
#include <endian.h> // __BYTE_ORDER __LITTLE_ENDIAN
#elif defined __FreeBSD__ || defined __OpenBSD__
#include <sys/endian.h> // _BYTE_ORDER _LITTLE_ENDIAN
#define __BYTE_ORDER _BYTE_ORDER
#define __LITTLE_ENDIAN _LITTLE_ENDIAN
#elif defined __MINGW32__
#include <sys/param.h> // BYTE_ORDER LITTLE_ENDIAN
#define __BYTE_ORDER BYTE_ORDER
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#endif

#include <algorithm> // std::reverse()

template <typename T>
constexpr T htonT(T value) noexcept
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return (std::reverse(reinterpret_cast<char *>(&value),
                         reinterpret_cast<char *>(&value) + sizeof(T)),
            value);
#else
    return value;
#endif
}

#if (!defined _WIN32) || (defined _WIN32 && _WIN32_WINNT < _WIN32_WINNT_WIN8)
inline uint64_t htonll(uint64_t value)
{
    return htonT<uint64_t>(value);
}

inline uint64_t ntohll(uint64_t value)
{
    return htonll(value);
}
#endif
#endif

namespace xiaoHttp
{
    namespace orm
    {
        enum class ClientType
        {
            PostgreSQL = 0,
            Mysql,
            Sqlite3
        };

        enum Sqlite3Type
        {
            Sqlite3TypeChar = 0,
            Sqlite3TypeShort,
            Sqlite3TypeInt,
            Sqlite3TypeInt64,
            Sqlite3TypeDouble,
            Sqlite3TypeText,
            Sqlite3TypeBlob,
            Sqlite3TypeNull
        };

        class DbClient;
        using QueryCallback = std::function<void(const Result &)>;
        using ExceptPtrCallback = std::function<void(const std::exception_ptr &)>;

        enum class Mode
        {
            NonBlocking,
            Blocking
        };

        namespace internal
        {
            template <typename T>
            struct VectorTypeTraits
            {
                static const bool isVector = false;
                static const bool isPtrVector = false;
                using ItemsType = T;
            };
        }

    }
}