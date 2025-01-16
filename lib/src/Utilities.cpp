/**
 * @file Utilities.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#include <xiaoHttp/utils/Utilities.h>
#include <xiaoLog/Date.h>

namespace xiaoHttp
{
    namespace utils
    {
        char *getHttpFullDate(const xiaoLog::Date &date)
        {
            static thread_local int64_t lastSecond = 0;
            static thread_local char lastTimeString[128] = {0};
            auto nowSecond = date.microSecondsSinceEpoch() / MICRO_SECONDS_PRE_SEC;
            if (nowSecond == lastSecond)
            {
                return lastTimeString;
            }
            lastSecond = nowSecond;
            date.toCustomFormattedString("%a, %d %b %Y %H:%M:%S GMT",
                                         lastTimeString,
                                         sizeof(lastTimeString));
            return lastTimeString;
        }

        std::string formattedString(const char *format, ...)
        {
            std::string strBuffer(128, 0);
            va_list ap, backup_ap;
            va_start(ap, format);
            va_copy(backup_ap, ap);
            auto result = vsnprintf((char *)strBuffer.data(),
                                    strBuffer.size(),
                                    format,
                                    backup_ap);
            va_end(backup_ap);
            if ((result >= 0) && ((std::string::size_type)result < strBuffer.size()))
            {
                strBuffer.resize(result);
            }
            else
            {
                while (true)
                {
                    if (result < 0)
                    {
                        strBuffer.resize(strBuffer.size() * 2);
                    }
                    else
                    {
                        strBuffer.resize(result + 1);
                    }

                    va_copy(backup_ap, ap);
                    auto result = vsnprintf((char *)strBuffer.data(),
                                            strBuffer.size(),
                                            format,
                                            backup_ap);
                    va_end(backup_ap);

                    if ((result >= 0) &&
                        ((std::string::size_type)result < strBuffer.size()))
                    {
                        strBuffer.resize(result);
                        break;
                    }
                }
            }
            va_end(ap);
            return strBuffer;
        }
    }
}