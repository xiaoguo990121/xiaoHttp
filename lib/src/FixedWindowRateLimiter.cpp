/**
 * @file FixedWindowRateLimiter.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-21
 *
 *
 */

#include "FixedWindowRateLimiter.h"

using namespace xiaoHttp;

FixedWindowRateLimiter::FixedWindowRateLimiter(
    size_t capacity,
    std::chrono::duration<double> timeUnit)
    : capacity_(capacity),
      lastTime_(std::chrono::steady_clock::now()),
      timeUnit_(timeUnit)
{
}
