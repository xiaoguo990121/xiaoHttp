/**
 * @file HttpAppFrameworkImpl.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#include "HttpAppFrameworkImpl.h"

using namespace xiaoHttp;

HttpAppFrameworkImpl::HttpAppFrameworkImpl()
    : listenerManagerPtr_(new ListenerManager),
      pluginsManagerPtr_(new PluginsManager),
      uploadPath_(rootPath_ + "uploads")
{
}