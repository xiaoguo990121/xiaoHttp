/**
 * @file CacheFile.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 *
 */

#pragma once

#include <xiaoNet/utils/NonCopyable.h>
#include <string>
#include <string_view>
#include <stdio.h>

namespace xiaoHttp
{
    class CacheFile : public xiaoNet::NonCopyable
    {
    public:
        explicit CacheFile(const std::string &path, bool autoDelete = true);

        ~CacheFile();

        void append(const std::string &data)
        {
            append(data.data(), data.length());
        }

        void append(const char *data, size_t length);

        std::string_view getStringView()
        {
            if (data())
                return std::string_view(data_, dataLength_);
            return std::string_view();
        }

    private:
        char *data();
        size_t length();
        FILE *file_{nullptr};
        bool autoDelete_{true};
        const std::string path_;
        char *data_{nullptr};
        size_t dataLength_{0};
    };
}