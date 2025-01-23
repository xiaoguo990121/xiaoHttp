/**
 * @file FunctionTraits.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 *
 */

#pragma once
#include <exception>
#include <string>
#include <tuple>
#include <type_traits>

namespace xiaoHttp
{
    namespace orm
    {
        class Result;
        class XiaoHttpDbException;

        namespace internal
        {
            template <typename>
            struct FunctionTraits;

            template <>
            struct FunctionTraits<void (*)()>
            {
                using result_type = void;
                template <std::size_t Index>
                using argument = void;
                static const std::size_t arity = 0;

                static const bool isSqlCallback = false;
                static const bool isExceptCallback = false;
            };

            template <typename Function>
            struct FunctionTraits
                : public FunctionTraits<
                      decltype(&std::remove_reference<Function>::type::operator())>
            {
            };

            template <typename ClassType, typename ReturnType, typename... Arguments>
            struct FunctionTraits<ReturnType (ClassType::*)(Arguments...) const>
                : FunctionTraits<ReturnType (*)(Arguments...)>
            {
            };

            template <typename ClassType, typename ReturnType, typename... Arguments>
            struct FunctionTraits<ReturnType (ClassType::*)(Arguments...)>
                : FunctionTraits<ReturnType (*)(Arguments...)>
            {
            };

            template <>
            struct FunctionTraits<void (*)(const Result &)>
                : public FunctionTraits<void (*)()>
            {
                static const bool isSqlCallback = true;
                static const bool isStepResultCallback = false;
                static const bool isExceptCallback = false;
            };

            template <>
            struct FunctionTraits<void (*)(const XiaoHttpDbException &)>
                : public FunctionTraits<void (*)()>
            {
                static const bool isExceptCallback = true;
                static const bool isSqlCallback = false;
                static const bool isStepResultCallback = false;
                static const bool isPtr = false;
            };

            template <>
            struct FunctionTraits<void (*)(const std::exception_ptr &)>
                : public FunctionTraits<void (*)()>
            {
                static const bool isExceptCallback = true;
                static const bool isSqlCallback = false;
                static const bool isStepResultCallback = false;
                static const bool isPtr = true;
            };

            template <typename ReturnType, typename... Arguments>
            struct FunctionTraits<ReturnType (*)(bool, Arguments...)>
                : FunctionTraits<ReturnType (*)(Arguments...)>
            {
                static const bool isSqlCallback = true;
                static const bool isStepResultCallback = true;
            };

            template <typename ReturnType, typename... Arguments>
            struct FunctionTraits<ReturnType (*)(Arguments...)>
            {
                using result_type = ReturnType;

                template <std::size_t Index>
                using argument =
                    typename std::tuple_element<Index, std::tuple<Arguments...>>::type;

                static const std::size_t arity = sizeof...(Arguments);

                static const bool isSqlCallback = true;
                static const bool isStepResultCallback = true;
                static const bool isExceptCallback = false;
                static const bool isPtr = false;
            };

        }
    }
}