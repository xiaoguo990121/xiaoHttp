/**
 * @file Utilities.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>

#include <xiaoLog/Funcs.h>
#include <xiaoLog/Date.h>

#include <xiaoNet/utils/Utilities.h>

#include <unordered_map>
#include <vector>
#include <set>
#include <stdexcept>

namespace xiaoHttp
{
    namespace internal
    {
        template <typename T, typename = void>
        struct CanConvertFromStringStream : std::false_type
        {
        };

        template <typename T>
        struct CanConvertFromStringStream<
            T,
            std::void_t<decltype(std::declval<std::stringstream &>() >>
                                 std::declval<T &>())>> : std::true_type
        {
        };

        template <typename T>
        struct CanConstructFromString : std::is_constructible<T, std::string>
        {
        };

        template <typename T>
        struct CanConvertFromString : std::is_assignable<T &, std::string>
        {
        };
    }

    XIAOHTTP_EXPORT const std::string_view &statusCodeToString(int code);

    namespace utils
    {
        /**
         * @brief Determine if the string is an integer
         *
         * @param std
         * @return XIAOHTTP_EXPORT
         */
        XIAOHTTP_EXPORT bool isInteger(std::string_view str);

        /**
         * @brief Determine if the string is base64 encoded
         *
         * @param str
         * @return XIAOHTTP_EXPORT
         */
        XIAOHTTP_EXPORT bool isBase64(std::string_view str);

        /**
         * @brief Generate random a string
         *
         * @param length
         * @return XIAOHTTP_EXPORT
         */
        XIAOHTTP_EXPORT std::string genRandomString(int length);

        XIAOHTTP_EXPORT std::string binaryStringToHex(const unsigned char *ptr,
                                                      size_t length,
                                                      bool lowerCase = false);

        XIAOHTTP_EXPORT std::string hexToBinaryString(const char *ptr, size_t length);

        XIAOHTTP_EXPORT std::vector<char> hexToBinaryVector(const char *ptr, size_t length);

        XIAOHTTP_EXPORT void binaryStringToHex(const char *ptr,
                                               size_t length,
                                               char *out,
                                               bool lowrCase = false);

        inline std::vector<std::string> splitString(const std::string &str,
                                                    const std::string &separator,
                                                    bool acceptEmptyString = false)
        {
            return xiaoLog::splitString(str, separator, acceptEmptyString);
        }

        XIAOHTTP_EXPORT std::set<std::string> splitStringToSet(
            const std::string &str,
            const std::string &separator);

        XIAOHTTP_EXPORT std::string getUuid(bool lowercase = true);

        constexpr size_t base64EncodedLength(size_t in_len, bool padded = true)
        {
            return padded ? ((in_len + 3 - 1) / 3) * 4 : (in_len * 8 + 6 - 1) / 6;
        }

        XIAOHTTP_EXPORT std::string base64Encode(const unsigned char *bytes_to_encode,
                                                 size_t in_len,
                                                 bool url_safe = false,
                                                 bool padded = true);

        inline std::string base64Encode(std::string_view data,
                                        bool url_safe = false,
                                        bool padded = true)
        {
            return base64Encode((unsigned char *)data.data(),
                                data.size(),
                                url_safe,
                                padded);
        }

        inline std::string base64EncodeUnpadded(const unsigned char *bytes_to_encode,
                                                size_t in_len,
                                                bool url_safe = false)
        {
            return base64Encode(bytes_to_encode, in_len, url_safe, false);
        }

        inline std::string base64EncodeUnpadded(std::string_view data,
                                                bool url_safe = false)
        {
            return base64Encode(data, url_safe, false);
        }

        constexpr size_t base64DecodedLength(size_t in_len)
        {
            return (in_len * 3) / 4;
        }

        XIAOHTTP_EXPORT std::string base64Decode(std::string_view encode_string);
        XIAOHTTP_EXPORT std::vector<char> base64DecodeToVector(
            std::string_view encoded_string);

        XIAOHTTP_EXPORT bool needUrlDecoding(const char *begin, const char *end);

        XIAOHTTP_EXPORT std::string urlDecode(const char *begin, const char *end);

        inline std::string urlDecode(const std::string &szToDecode)
        {
            auto begin = szToDecode.data();
            return urlDecode(begin, begin + szToDecode.length());
        }

        inline std::string urlDecode(const std::string_view &szToDecode)
        {
            auto begin = szToDecode.data();
            return urlDecode(begin, begin + szToDecode.length());
        }

        XIAOHTTP_EXPORT std::string urlEncode(const std::string &);
        XIAOHTTP_EXPORT std::string urlEncodeComponent(const std::string &);

        XIAOHTTP_EXPORT std::string getMd5(const char *data, const size_t dataLen);

        inline std::string getMd5(const std::string &originalString)
        {
            return getMd5(originalString.data(), originalString.length());
        }

        XIAOHTTP_EXPORT std::string getSha1(const char *data, const size_t dataLen);

        inline std::string getSha1(const std::string &originalString)
        {
            return getSha1(originalString.data(), originalString.length());
        }

        XIAOHTTP_EXPORT std::string getSha256(const char *data, const size_t dataLen);

        inline std::string getSha256(const std::string &originalString)
        {
            return getSha256(originalString.data(), originalString.length());
        }

        XIAOHTTP_EXPORT std::string getSha3(const char *data, const size_t dataLen);

        inline std::string getSha3(const std::string &originalString)
        {
            return getSha3(originalString.data(), originalString.length());
        }

        XIAOHTTP_EXPORT std::string getBlake2b(const char *data, const size_t dataLen);

        inline std::string getBlake2b(const std::string &originalString)
        {
            return getBlake2b(originalString.data(), originalString.length());
        }

        /**
         * @brief Compress or decompress data using gzip lib.
         *
         * @param data
         * @param ndata
         * @return XIAOHTTP_EXPORT
         */
        XIAOHTTP_EXPORT std::string gzipCompress(const char *data, const size_t ndata);
        XIAOHTTP_EXPORT std::string gzipDecompress(const char *data, const size_t ndata);

        /**
         * @brief Compress or decompress data using brotli lib.
         *
         * @param data
         * @param ndata
         * @return XIAOHTTP_EXPORT
         */
        XIAOHTTP_EXPORT std::string brotilCompress(const char *data, const size_t ndata);
        XIAOHTTP_EXPORT std::string brotilDecompress(const char *data, const size_t ndata);

        /// Get the http full date string
        /**
         * rfc2616-3.3.1
         * Full Date format(RFC 822)
         * like this:
         * @code
           Sun, 06 Nov 1994 08:49:37 GMT
           Wed, 12 Sep 2018 09:22:40 GMT
           @endcode
         */
        XIAOHTTP_EXPORT char *getHttpFullDate(
            const xiaoLog::Date &date = xiaoLog::Date::now());

        XIAOHTTP_EXPORT xiaoLog::Date getHttpDate(const std::string &httpFullDateString);

        /**
         * @brief Get a formatted string
         *
         * @param format
         * @param ...
         * @return XIAOHTTP_EXPORT
         */
        XIAOHTTP_EXPORT std::string formattedString(const char *format, ...);

        XIAOHTTP_EXPORT int createPath(const std::string &path);

        inline std::string fromWidePath(const std::wstring &strPath)
        {
            return xiaoNet::utils::fromWidePath(strPath);
        }

        inline std::wstring toWidePath(const std::string &strUtf8Path)
        {
            return xiaoNet::utils::toWidePath(strUtf8Path);
        }

#if defined(_WIN32) && !defined(__MINGW32__)
        inline std::wstring toNativePath(const std::string &strPath)
        {
            return trantor::utils::toNativePath(strPath);
        }

        inline const std::wstring &toNativePath(const std::wstring &strPath)
        {
            return trantor::utils::toNativePath(strPath);
        }
#else  // __WIN32
        inline const std::string &toNativePath(const std::string &strPath)
        {
            return xiaoNet::utils::toNativePath(strPath);
        }

        inline std::string toNativePath(const std::wstring &strPath)
        {
            return xiaoNet::utils::toNativePath(strPath);
        }
#endif // _WIN32

        inline const std::string &fromNativePath(const std::string &strPath)
        {
            return xiaoNet::utils::fromNativePath(strPath);
        }

        // Convert on all systems
        inline std::string fromNativePath(const std::wstring &strPath)
        {
            return xiaoNet::utils::fromNativePath(strPath);
        }

        /// Replace all occurrences of from to to inplace
        /**
         * @param s string to alter
         * @param from string to replace
         * @param to string to replace with
         */
        XIAOHTTP_EXPORT void replaceAll(std::string &s,
                                        const std::string &from,
                                        const std::string &to);

        XIAOHTTP_EXPORT bool secureRandomBytes(void *ptr, size_t size);

        XIAOHTTP_EXPORT std::string secureRandomString(size_t size);

        template <typename T>
        T fromString(const std::string &p) noexcept(false)
        {
            if constexpr (std::is_integral<T>::value && std::is_signed<T>::value)
            {
                std::size_t pos;
                auto v = std::stoll(p, &pos);
                // throw if the whole string could not be parsed
                // ("1a" should not return 1)
                if (pos != p.size())
                    throw std::invalid_argument("Invalid value");
                if ((v < static_cast<long long>((std::numeric_limits<T>::min)())) ||
                    (v > static_cast<long long>((std::numeric_limits<T>::max)())))
                    throw std::out_of_range("Value out of range");
                return static_cast<T>(v);
            }
            else if constexpr (std::is_integral<T>::value &&
                               (!std::is_signed<T>::value))
            {
                std::size_t pos;
                auto v = std::stoull(p, &pos);
                // throw if the whole string could not be parsed
                // ("1a" should not return 1)
                if (pos != p.size())
                    throw std::invalid_argument("Invalid value");
                if (v >
                    static_cast<unsigned long long>((std::numeric_limits<T>::max)()))
                    throw std::out_of_range("Value out of range");
                return static_cast<T>(v);
            }
            else if constexpr (std::is_floating_point<T>::value)
            {
                std::size_t pos;
                auto v = std::stold(p, &pos);
                // throw if the whole string could not be parsed
                // ("1a" should not return 1)
                if (pos != p.size())
                    throw std::invalid_argument("Invalid value");
                if ((v < static_cast<long double>((std::numeric_limits<T>::min)())) ||
                    (v > static_cast<long double>((std::numeric_limits<T>::max)())))
                    throw std::out_of_range("Value out of range");
                return static_cast<T>(v);
            }
            else if constexpr (internal::CanConvertFromStringStream<T>::value)
            {
                T value{};
                if (!p.empty())
                {
                    std::stringstream ss(p);
                    // must except in case of invalid value, not return a default value
                    // (else it returns 0 for integers if the string is empty or
                    // non-numeric)
                    ss.exceptions(std::ios_base::failbit);
                    ss >> value;
                    // throw if the whole string could not be parsed
                    // ("1a" should not return 1)
                    if (!ss.eof())
                        std::runtime_error("Bad type conversion");
                }
                return value;
            }
            else
            {
                throw std::runtime_error("Bad type conversion");
            }
        }

        template <>
        inline std::string fromString<std::string>(const std::string &p) noexcept(false)
        {
            return p;
        }

        template <>
        inline bool fromString<bool>(const std::string &p) noexcept(false)
        {
            if (!p.empty() && std::all_of(p.begin(), p.end(), [](unsigned char c)
                                          { return std::isdigit(c); }))
                return (std::stoll(p) != 0);
            std::string l{p};
            std::transform(p.begin(), p.end(), l.begin(), [](unsigned char c)

                           { return (char)tolower(c); });
            if (l == "true")
            {
                return true;
            }
            else if (l == "false")
            {
                return false;
            }
            throw std::runtime_error("Can't convert from string '" + p + "' to bool");
        }

        XIAOHTTP_EXPORT bool supportsTls() noexcept;

        namespace internal
        {
            XIAOHTTP_EXPORT extern const size_t fixedRandomNumber;

            struct SafeStringHash
            {
                size_t operator()(const std::string &str) const
                {
                    const size_t A = 6665339;
                    const size_t B = 2534641;
                    size_t h = fixedRandomNumber;
                    for (char ch : str)
                        h = (h * A) ^ (ch * B);
                    return h;
                }
            };
        }
    }

    template <typename T>
    using SafeStringMap =
        std::unordered_map<std::string, T, utils::internal::SafeStringHash>;
}
