/**
 * @file CacheFile.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 *
 */

#include "CacheFile.h"
#include <xiaoLog/Logger.h>

#ifdef _WIN32

#else
#include <unistd.h>
#include <sys/mman.h>
#endif

using namespace xiaoHttp;

CacheFile::CacheFile(const std::string &path, bool autoDelete)
    : autoDelete_(autoDelete), path_(path)
{
#ifndef _MSC_VER
    file_ = fopen(path_.data(), "wb+");
#else
#endif
}

CacheFile::~CacheFile()
{
    if (data_)
    {
        munmap(data_, dataLength_);
    }
    if (autoDelete_ && file_)
    {
        fclose(file_);
#if defined(_WIN32) && !defined(__MINGW32__)
#else
        unlink(path_.data());
#endif
    }
    else if (file_)
    {
        fclose(file_);
    }
}

void CacheFile::append(const char *data, size_t length)
{
    if (file_)
        fwrite(data, length, 1, file_);
}

size_t CacheFile::length()
{
    if (file_)
#ifdef _WIN32
#else
        return ftell(file_);
#endif
        return 0;
}

char *CacheFile::data()
{
    if (!file_)
        return nullptr;
    if (!data_)
    {
        fflush(file_);
#ifdef _WIN32
#else
        auto fd = fileno(file_);
#endif
        dataLength_ = length();
        data_ = static_cast<char *>(
            mmap(nullptr, dataLength_, PROT_READ, MAP_SHARED, fd, 0));
        if (data_ == MAP_FAILED)
        {
            data_ = nullptr;
            LOG_SYSERR << "mmap: ";
        }
    }
    return data_;
}