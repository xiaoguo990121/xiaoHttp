/**
 * @file DbClient.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>

#include <functional>

namespace xiaoHttp
{
    namespace orm
    {
        using ResultCallback = std::function<void(const Result &)>;
        using ExceptionCallback = std::function < void(const)
    }
}