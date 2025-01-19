/**
 * @file xiaoHttp_test.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#pragma once

#include <xiaoNet/utils/NonCopyable.h>
#include <xiaoHttp/DrObject.h>
#include <xiaoHttp/exports.h>

#include <memory>
#include <mutex>
#include <sstream>
#include <atomic>
#include <string_view>
#include <cstddef>
#include <iostream>

/**
 * @brief XiaoHttp Test is a minimal effort test framework developed because the
 * major C++ frameworks doesn't handle async programs well. XiaoHttp Test's syntax
 * is inspired by both Google Test and Catch2
 *
 */
namespace xiaoHttp
{
    namespace test
    {
#define TEST_CTX xiaoHttp_test_ctx_
#define XIAOHTTP_TESTCASE_PREIX_ xhtest__
#define XIAOHTTP_TESTCASE_PREIS_STR_ "xhtest__"
#define TEST_FLAG_ xhgood_

#define XIAOHTTP_TEST_STRINGIFY__(x) #x
#define XIAOHTTP_TEST_STRINGIFY(x) XIAOHTTP_TEST_STRINGIFY__(x)
#define XIAOHTTP_TEST_CONCAT__(a, b) a##b // 通常__的宏是底层实现宏
#define XIAOHTTP_TEST_CONCAT(a, b) XIAOHTTP_TEST_CONCAT__(a, b)
        // https://www.zhihu.com/question/353259735#:~:text=%E5%A4%9A%E5%AE%9A%E4%B9%89%E4%B8%80%E5%B1%82%EF%BC%8C%E5%8F%AF%E4%BB%A5%E4%BF%9D%E8%AF%81a%2Cb%E5%9C%A8%E4%B8%8A%E4%B8%80%E5%B1%82%E5%B1%95%E5%BC%80%EF%BC%8C%E6%9C%80%E5%90%8E%E5%8F%AA%E5%81%9A%E7%AC%A6%E5%8F%B7%E6%8B%BC%E6%8E%A5%E3%80%82%20foo2%20%28foo2%20%28foo%29%29%20%E4%BC%9A%E5%B1%95%E5%BC%80%E4%B8%BA%20foo2%20%28abc%29123%EF%BC%8C%E5%9B%A0%E4%B8%BA%E7%AC%A6%E5%8F%B7%E8%BF%9E%E6%8E%A5%E4%BA%86%EF%BC%8C%E8%BF%99%E6%98%AF%E4%B8%80%E4%B8%AA%E7%AC%A6%E5%8F%B7%EF%BC%8C%E5%89%8D%E9%9D%A2%E9%83%A8%E5%88%86%E4%B8%8D%E4%BC%9A%E5%B1%95%E5%BC%80%E3%80%82%20%E5%B0%86%E4%BC%9A%E9%98%BB%E6%AD%A2%E5%8F%A6%E4%B8%80%E4%B8%AA%E5%AE%8F%E7%9A%84%E5%B1%95%E5%BC%80%E3%80%82,%E7%BB%93%E6%9E%9C%E6%98%AF%20foo123%20%E3%80%82%20%E5%A6%82%E6%9E%9C%E5%AE%8F%E4%B8%AD%E6%B2%A1%E6%9C%89%20%23%20%E6%88%96%E8%80%85%20%23%23%EF%BC%8C%20%E5%85%88%E5%AF%B9%E5%8F%82%E6%95%B0%E5%86%8D%E8%BF%9B%E8%A1%8C%E6%9B%BF%E6%8D%A2%EF%BC%8C%E6%9C%80%E5%86%85%E5%B1%82%E7%9A%84%E5%AE%8F%E4%BC%9A%E5%85%88%E8%A2%AB%E6%89%A9%E5%B1%95%E3%80%82
        // 为什么要这么设计
        // 一旦在使用 # 和 ## 的宏中， 其中一个宏参数是另一个宏将会阻止另一个宏展开
        // 如果是 XIAOHTTP_TEST_CONCAT(a,b) a##b

#if defined(__GNUC__) && !defined(__clang__) && !defined(__ICC)
#define XIAOHTTP_TEST_START_SUPRESSION_ _Pragma("GCC diagnostic push")
#define XIAOHTTP_TEST_END_SUPRESSION_ _Pragma("GCC diagnostic pop")
#define XIAOHTTP_TEST_SUPPRESS_PARENTHESES_WARNING_ \
    _Pragma("GCC diagnostic ignored \"-Wparentheses\"")
#define XIAOHTTP_TEST_SUPPRESS_UNUSED_VALUE_WARNING_ \
    _Pragma("GCC diagnostic ignored \"-Wunused-value\"")
#elif defined(__clang__) && !defined(_MSC_VER)
#define XIAOHTTP_TEST_START_SUPRESSION_ _Pragma("clang diagnostic push")
#define XIAOHTTP_TEST_END_SUPRESSION_ _Pragma("clang diagnostic pop")
#define XIAOHTTP_TEST_SUPPRESS_PARENTHESES_WARNING_ \
    _Pragma("clang diagnostic ignored \"-Wparentheses\"")
#define XIAOHTTP_TEST_SUPPRESS_UNUSED_VALUE_WARNING_ \
    _Pragma("clang diagnostic ignored \"-Wunused-value\"")
// MSVC don't have an equlivent. Add other compilers here
#else
#define XIAOHTTP_TEST_START_SUPRESSION_
#define XIAOHTTP_TEST_END_SUPRESSION_
#define XIAOHTTP_TEST_SUPPRESS_PARENTHESES_WARNING_
#define XIAOHTTP_TEST_SUPPRESS_UNUSED_VALUE_WARNING_
#endif

        class Case;

        namespace internal
        {
            XIAOHTTP_EXPORT extern std::atomic<size_t> numAssertions;
            XIAOHTTP_EXPORT extern std::atomic<size_t> numCorrectAssertions;
            XIAOHTTP_EXPORT extern std::atomic<size_t> numFailedTestCases;
            XIAOHTTP_EXPORT extern bool printSuccessfulTests;

            XIAOHTTP_EXPORT void registerCase(Case *test);
            XIAOHTTP_EXPORT void unregisterCase(Case *test);

            template <typename _Tp, typename dummpy = void>
            struct is_printable : std::false_type
            {
            };

            template <typename _Tp>
            struct is_printable<
                _Tp,
                std::enable_if_t<std::is_same_v<decltype(std::cout << std::declval<_Tp>()),
                                                std::ostream &>>> : std::true_type
            {
            };

            inline std::string escapeString(const std::string_view sv)
            {
                std::string result;
                result.reserve(sv.size());
                for (auto ch : sv)
                {
                    if (ch == '\n')
                        result += "\\n";
                    else if (ch == '\r')
                        result += "\\r";
                    else if (ch == '\t')
                        result += "\\t";
                    else if (ch == '\b')
                        result += "\\b";
                    else if (ch == '\\')
                        result += "\\\\";
                    else if (ch == '"')
                        result += "\"";
                    else if (ch == '\v')
                        result += "\\v";
                    else if (ch == '\a')
                        result += "\\a";
                    else
                        result.push_back(ch);
                }
                return result;
            }

            XIAOHTTP_EXPORT std::string prettifyString(const std::string_view sv,
                                                       size_t maxLength = 120);

            // 折叠表达式
            template <typename... Args>
            inline void outputReason(Args &&...args)
            {
                (std::cout << ... << std::forward<Args>(args));
            }

            template <typename T>
            inline std::string attemptPrint(T &&v)
            {
                using Type = std::remove_cv_t<std::remove_reference_t<T>>;
                if constexpr (std::is_same_v<Type, std::nullptr_t>)
                    return "nullptr";
                else if constexpr (std::is_same_v<Type, char>)
                    return "'" + std::string(1, v) + "'";
                else if constexpr (std::is_convertible_v<Type, std::string_view>)
                    return prettifyString(v);
                else if constexpr (internal::is_printable<Type>::value)
                {
                    std::stringstream ss;
                    ss << v;
                    return ss.str();
                }
                return "{un-printable}";
            }

            inline std::string stringifyFuncCall(const std::string &funcName)
            {
                return funcName + "()";
            }

            inline std::string stringifyFuncCall(const std::string &funcName,
                                                 const std::string &param1)
            {
                return funcName + "(" + param1 + ")";
            }

            inline std::string stringifyFuncCall(const std::string &funcName,
                                                 const std::string &param1,
                                                 const std::string &param2)
            {
                return funcName + "(" + param1 + ", " + param2 + ")";
            }

            struct ComparsionResult // 比较结果的结构体
            {
                std::pair<bool, std::string> result() const
                {
                    return {comparsionResult_, expansion_};
                }

                bool comparsionResult_;
                std::string expansion_;
            };

            template <typename T>
            struct Lhs
            {
                template <typename _ = void>
                std::pair<bool, std::string> result() const
                {
                    return {(bool)ref_, attemptPrint(ref_)};
                }

                Lhs(const T &lhs) : ref_(lhs)
                {
                }

                const T &ref_; // 对传入左值的引用，表示比较中的左操作数

                template <typename RhsType>
                ComparsionResult operator<(const RhsType &rhs)
                {
                    return ComparsionResult{ref_ < rhs,
                                            attemptPrint(ref_) + " < " +
                                                attemptPrint(ref_)};
                }

                template <typename RhsType>
                ComparsionResult operator>(const RhsType &rhs)
                {
                    return ComparsionResult{ref_ > rhs,
                                            attemptPrint(ref_) + " > " + attemptPrint(rhs)};
                }

                template <typename RhsType>
                ComparsionResult operator<=(const RhsType &rhs)
                {
                    return ComparsionResult{ref_ <= rhs,
                                            attemptPrint(ref_) +
                                                " <= " + attemptPrint(rhs)};
                }

                template <typename RhsType>
                ComparsionResult operator>=(const RhsType &rhs)
                {
                    return ComparsionResult{ref_ >= rhs,
                                            attemptPrint(ref_) +
                                                " >= " + attemptPrint(rhs)};
                }

                template <typename RhsType>
                ComparsionResult operator==(const RhsType &rhs)
                {
                    return ComparsionResult{ref_ == rhs,
                                            attemptPrint(ref_) +
                                                " == " + attemptPrint(rhs)};
                }

                template <typename RhsType>
                ComparsionResult operator!=(const RhsType &rhs)
                {
                    return ComparsionResult{ref_ != rhs,
                                            attemptPrint(ref_) +
                                                " != " + attemptPrint(rhs)};
                }

                template <typename RhsType>
                ComparsionResult operator&&(const RhsType &rhs)
                {
                    static_assert(!std::is_same_v<RhsType, void>,
                                  " && is not supported in expression decomposition");
                    return {};
                }

                template <typename RhsType>
                ComparsionResult operator||(const RhsType &rhs)
                {
                    static_assert(!std::is_same_v<RhsType, void>,
                                  " || is not supported in expression decomposition");
                    return {};
                }

                template <typename RhsType>
                ComparsionResult operator|(const RhsType &rhs)
                {
                    static_assert(!std::is_same_v<RhsType, void>,
                                  " | is not supported in expression decomposition");
                    return {};
                }

                template <typename RhsType>
                ComparsionResult operator&(const RhsType &rhs)
                {
                    static_assert(!std::is_same_v<RhsType, void>,
                                  " & is not supported in expression decomposition");
                    return {};
                }
            };

            struct Decomposer
            {
                template <typename T>
                Lhs<T> operator<=(const T &other)
                {
                    return Lhs<T>(other);
                }
            };
        }

        class XIAOHTTP_EXPORT ThreadSafeStream final
        {
        public:
            ThreadSafeStream(std::ostream &os) : os_(os)
            {
                mtx_.lock();
            }

            ~ThreadSafeStream()
            {
                mtx_.unlock();
            }

            template <typename T>
            std::ostream &operator<<(const T &rhs)
            {
                return os_ << rhs;
            }

            static std::mutex mtx_;
            std::ostream &os_;
        };

        XIAOHTTP_EXPORT ThreadSafeStream print();
        XIAOHTTP_EXPORT ThreadSafeStream printErr();

        class CaseBase : public xiaoNet::NonCopyable
        {
        public:
            CaseBase() = default;

            CaseBase(const std::string &name) : name_(name)
            {
            }

            CaseBase(std::shared_ptr<CaseBase> parent, const std::string &name)
                : parent_(parent), name_(name)
            {
            }

            virtual ~CaseBase() = default;

            std::string fullname() const
            {
                std::string result;
                auto curr = this;
                while (curr != nullptr)
                {
                    result = curr->name() + result;
                    if (curr->parent_ != nullptr)
                        result = "." + result;
                    curr = curr->parent_.get();
                }
            }

            const std::string &name() const
            {
                return name_;
            }

            void setFailed()
            {
                if (failed_ == false)
                {
                    internal::numFailedTestCases++;
                    failed_ = true;
                }
            }

            bool failed() const
            {
                return failed_;
            }

        protected:
            bool failed_ = false;
            std::shared_ptr<CaseBase> parent_ = nullptr;
            std::string name_;
        };

        class Case : public CaseBase
        {
        public:
            Case(const std::string &name) : CaseBase(name)
            {
                internal::registerCase(this);
            }

            Case(std::shared_ptr<Case> parent, const std::string &name)
                : CaseBase(parent, name)
            {
                internal::registerCase(this);
            }

            virtual ~Case()
            {
                internal::unregisterCase(this);
            }
        };

        struct TestCase : public CaseBase
        {
            TestCase(const std::string &name) : CaseBase(name)
            {
            }

            virtual ~TestCase() = default;
            virtual void doTest_(std::shared_ptr<Case>) = 0;
        };

        XIAOHTTP_EXPORT void printTestStats();
        XIAOHTTP_EXPORT int run(int argc, char **argv);
    }
}

#define ERROR_MSG(func_name, expr)                                      \
    xiaoHttp::test::printErr()                                          \
        << "\x1B[1;37mIn test case " << TEST_CTX->fullname() << "\n"    \
        << "\x1B[0;37m↳ " << __FILE__ << ":" << __LINE__                \
        << " \x1B[0;31m FAILED:\x1B[0m\n"                               \
        << "  \033[0;34m"                                               \
        << xiaoHttp::test::internal::stringifyFuncCall(func_name, expr) \
        << "\x1B[0m\n"

#define PASSED_MSG(func_name, expr)                                     \
    xiaoHttp::test::print()                                             \
        << "\x1B[1;37mIn test case " << TEST_CTX->fullname() << "\n"    \
        << "\x1B[0;37m↳ " << __FILE__ << ":" << __LINE__                \
        << " \x1B[0;32m PASSED:\x1B[0m\n"                               \
        << "  \033[0;34m"                                               \
        << xiaoHttp::test::internal::stringifyFuncCall(func_name, expr) \
        << "\x1B[0m\n"

#define SET_TEST_SUCCESS__ \
    do                     \
    {                      \
        TEST_FLAG_ = true; \
    } while (0);

#define TEST_INTERNAL__(func_name,                            \
                        expr,                                 \
                        eval,                                 \
                        on_exception,                         \
                        on_non_standard_exception,            \
                        on_leaving)                           \
    do                                                        \
    {                                                         \
        bool TEST_FLAG_ = false;                              \
        using xiaoHttp::test::internal::stringifyFuncCall;    \
        using xiaoHttp::test::printErr;                       \
        xiaoHttp::test::internal::numAssertions++;            \
        try                                                   \
        {                                                     \
            eval;                                             \
        }                                                     \
        catch (const std::exception &e)                       \
        {                                                     \
            (void)e;                                          \
            on_exception;                                     \
        }                                                     \
        catch (...)                                           \
        {                                                     \
            on_non_standard_exception;                        \
        }                                                     \
        if (TEST_FLAG_)                                       \
            xiaoHttp::test::internal::numCorrectAssertions++; \
        else                                                  \
            TEST_CTX->setFailed();                            \
        on_leaving;                                           \
    } while (0);

#define EVAL_AND_CHECK_TRUE__(func_name, expr)                         \
    do                                                                 \
    {                                                                  \
        bool xhresult__;                                               \
        std::string xhexpansion__;                                     \
        XIAOHTTP_TEST_START_SUPRESSION_                                \
        XIAOHTTP_TEST_SUPPRESS_PARENTHESES_WARNING_                    \
        std::tie(xhresult__, xhexpansion__) =                          \
            (xiaoHttp::test::internal::Decomposer() <= expr).result(); \
        XIAOHTTP_TEST_END_SUPRESSION_                                  \
        if (!xhresult__)                                               \
        {                                                              \
            ERROR_MSG(func_name, #expr)                                \
                << "With expansion\n"                                  \
                << "  \033[0;33m" << xhexpansion__ << "\x1B[0m\n\n";   \
        }                                                              \
        else                                                           \
            SET_TEST_SUCCESS__;                                        \
    } while (0);

#define PRINT_UNEXPECTED_EXCEPTION__(func_name, expr)         \
    do                                                        \
    {                                                         \
        ERROR_MSG(func_name, expr)                            \
            << "An unexpected exception is thrown. what():\n" \
            << "  \033[0;33m" << e.what() << "\x1B[0m\n\n";   \
    } while (0);

#define PRINT_PASSED__(func_name, expr)                                   \
    do                                                                    \
    {                                                                     \
        if (xiaoHttp::test::internal::printSuccessfulTests && TEST_FLAG_) \
        {                                                                 \
            PASSED_MSG(func_name, expr) << "\n";                          \
        }                                                                 \
    } while (0);

#define RETURN_ON_FAILURE__ \
    do                      \
    {                       \
        if (!TEST_FLAG_)    \
            return;         \
    } while (0);

#define CO_RETURN_ON_FAILURE__ \
    do                         \
    {                          \
        if (!TEST_FLAG_)       \
            co_return;         \
    } while (0);

#define DIE_ON_FAILURE__                                                \
    do                                                                  \
    {                                                                   \
        using namespace xiaoHttp::test;                                 \
        if (!TEST_FLAG_)                                                \
        {                                                               \
            printTestStats();                                           \
            printErr() << "Force exiting due to a mandation failed.\n"; \
            exit(1);                                                    \
        }                                                               \
    } while (0);

#define PRINT_NONSTANDARD_EXCEPTION__(func_name, expr)        \
    do                                                        \
    {                                                         \
        ERROR_MSG(func_name, expr)                            \
            << "Unexpected unknown exception is thrown.\n\n"; \
    } while (0);

#define EVAL__(expr) \
    do               \
    {                \
        expr;        \
    } while (0);

#define NOTHING__ \
    {             \
    }

#define PRINT_ERR_NOEXCEPTION__(expr, func_name)                     \
    do                                                               \
    {                                                                \
        if (!TEST_FLAG_)                                             \
            ERROR_MSG(func_name, expr)                               \
                << "With expecitation\n"                             \
                << "  Expected to throw an exception. But none are " \
                   "thrown.\n\n";                                    \
    } while (0);

#define PRINT_ERR_WITHEXCEPTION__(expr, func_name)                   \
    do                                                               \
    {                                                                \
        if (!TEST_FLAG_)                                             \
            ERROR_MSG(func_name, expr)                               \
                << "With expecitation\n"                             \
                << "  Should to not throw an exception. But one is " \
                   "thrown.\n\n";                                    \
    } while (0);

#define PRINT_ERR_BAD_EXCEPTION__(                                             \
    expr, func_name, excep_type, exceptionThrown, correctExceptionType)        \
    do                                                                         \
    {                                                                          \
        assert((exceptionThrown && correctExceptionType) || !exceptionThrown); \
        if (exceptionThrown == true && correctExceptionType == false)          \
        {                                                                      \
            ERROR_MSG(func_name, expr)                                         \
                << "With expecitation\n"                                       \
                << "  Exception have been throw but not of type \033[0;33m"    \
                << #excep_type << "\033[0m.\n\n";                              \
        }                                                                      \
        else if (exceptionThrown == false)                                     \
        {                                                                      \
            ERROR_MSG(func_name, expr)                                         \
                << "With expecitation\n"                                       \
                << "  A \033[0;33m" << #excep_type                             \
                << "\033[0m exception is expected. But nothing was thrown"     \
                << "\033[0m.\n\n";                                             \
        }                                                                      \
    } while (0);

#define CHECK_INTERNAL__(expr, func_name, on_leave)                      \
    do                                                                   \
    {                                                                    \
        TEST_INTERNAL__(func_name,                                       \
                        expr,                                            \
                        EVAL_AND_CHECK_TRUE__(func_name, expr),          \
                        PRINT_UNEXPECTED_EXCEPTION__(func_name, #expr),  \
                        PRINT_NONSTANDARD_EXCEPTION__(func_name, #expr), \
                        on_leave PRINT_PASSED__(func_name, #expr));      \
    } while (0)

#define CHECK_THROWS_INTERNAL__(expr, func_name, on_leave)              \
    do                                                                  \
    {                                                                   \
        TEST_INTERNAL__(func_name,                                      \
                        expr,                                           \
                        EVAL__(expr),                                   \
                        SET_TEST_SUCCESS__,                             \
                        SET_TEST_SUCCESS__,                             \
                        PRINT_ERR_NOEXCEPTION__(#expr, func_name)       \
                            on_leave PRINT_PASSED__(func_name, #expr)); \
    } while (0)

#define CHECK_THROWS_AS_INTERNAL__(expr, func_name, except_type, on_leave)    \
    do                                                                        \
    {                                                                         \
        bool exceptionThrown = false;                                         \
        TEST_INTERNAL__(                                                      \
            func_name,                                                        \
            expr,                                                             \
            EVAL__(expr),                                                     \
            {                                                                 \
                exceptionThrown = true;                                       \
                if (dynamic_cast<const except_type *>(&e) != nullptr)         \
                    SET_TEST_SUCCESS__;                                       \
            },                                                                \
            { exceptionThrown = true; },                                      \
            PRINT_ERR_BAD_EXCEPTION__(#expr ", " #except_type,                \
                                      func_name,                              \
                                      except_type,                            \
                                      exceptionThrown,                        \
                                      TEST_FLAG_)                             \
                on_leave PRINT_PASSED__(func_name, #expr ", " #except_type)); \
    } while (0)

#define CHECK_NOTHROW_INTERNAL__(expr, func_name, on_leave)             \
    do                                                                  \
    {                                                                   \
        TEST_INTERNAL__(func_name,                                      \
                        expr,                                           \
                        EVAL__(expr) SET_TEST_SUCCESS__,                \
                        NOTHING__,                                      \
                        NOTHING__,                                      \
                        PRINT_ERR_WITHEXCEPTION__(#expr, func_name)     \
                            on_leave PRINT_PASSED__(func_name, #expr)); \
    } while (0)

#define CHECK(expr) CHECK_INTERNAL__(expr, "CHECK", NOTHING__)
#define CHECK_THROWS(expr) \
    CHECK_THROWS_INTERNAL__(expr, "CHECK_THROWS", NOTHING__)
#define CHECK_NOTHROW(expr) \
    CHECK_NOTHROW_INTERNAL__(expr, "CHECK_NOTHROW", NOTHING__)
#define CHECK_THROWS_AS(expr, except_type) \
    CHECK_THROWS_AS_INTERNAL__(expr, "CHECK_THROWS_AS", except_type, NOTHING__)

#define REQUIRE(expr) CHECK_INTERNAL__(expr, "REQUIRE", RETURN_ON_FAILURE__)
#define REQUIRE_THROWS(expr) \
    CHECK_THROWS_INTERNAL__(expr, "REQUIRE_THROWS", RETURN_ON_FAILURE__)
#define REQUIRE_NOTHROW(expr) \
    CHECK_NOTHROW_INTERNAL__(expr, "REQUIRE_NOTHROW", RETURN_ON_FAILURE__)
#define REQUIRE_THROWS_AS(expr, except_type)        \
    CHECK_THROWS_AS_INTERNAL__(expr,                \
                               "REQUIRE_THROWS_AS", \
                               except_type,         \
                               RETURN_ON_FAILURE__)

#define CO_REQUIRE(expr) \
    CHECK_INTERNAL__(expr, "CO_REQUIRE", CO_RETURN_ON_FAILURE__)
#define CO_REQUIRE_THROWS(expr) \
    CHECK_THROWS_INTERNAL__(expr, "CO_REQUIRE_THROWS", CO_RETURN_ON_FAILURE__)
#define CO_REQUIRE_NOTHROW(expr) \
    CHECK_NOTHROW_INTERNAL__(expr, "CO_REQUIRE_NOTHROW", CO_RETURN_ON_FAILURE__)
#define CO_REQUIRE_THROWS_AS(expr, except_type)        \
    CHECK_THROWS_AS_INTERNAL__(expr,                   \
                               "CO_REQUIRE_THROWS_AS", \
                               except_type,            \
                               CO_RETURN_ON_FAILURE__)

#define MANDATE(expr) CHECK_INTERNAL__(expr, "MANDATE", DIE_ON_FAILURE__)
#define MANDATE_THROWS(expr) \
    CHECK_THROWS_INTERNAL__(expr, "MANDATE_THROWS", DIE_ON_FAILURE__)
#define MANDATE_NOTHROW(expr) \
    CHECK_NOTHROW_INTERNAL__(expr, "MANDATE_NOTHROW", DIE_ON_FAILURE__)
#define MANDATE_THROWS_AS(expr, except_type)        \
    CHECK_THROWS_AS_INTERNAL__(expr,                \
                               "MANDATE_THROWS_AS", \
                               except_type,         \
                               DIE_ON_FAILURE__)

#define STATIC_REQUIRE(expr)                              \
    do                                                    \
    {                                                     \
        XIAOHTTP_TEST_START_SUPRESSION_                   \
        XIAOHTTP_TEST_SUPPRESS_UNUSED_VALUE_WARNING_      \
        TEST_CTX;                                         \
        XIAOHTTP_TEST_END_SUPRESSION_                     \
        xiaoHttp::test::internal::numAssertions++;        \
        static_assert((expr), #expr " failed.");          \
        xiaoHttp::test::internal::numCorrectAssertions++; \
    } while (0);

#define FAIL(...)                                                       \
    do                                                                  \
    {                                                                   \
        using namespace xiaoHttp::test;                                 \
        TEST_CTX->setFailed();                                          \
        printErr() << "\x1B[1;37mIn test case " << TEST_CTX->fullname() \
                   << "\n"                                              \
                   << "\x1B[0;37m" << __FILE__ << ":" << __LINE__       \
                   << " \x1B[0;31m FAILED:\x1B[0m\n"                    \
                   << "  Reason: ";                                     \
        xiaoHttp::test::internal::outputReason(__VA_ARGS__);            \
        printErr() << "\n\n";                                           \
        xiaoHttp::test::internal::numAssertions++;                      \
    } while (0)

#define FAULT(...)                                                 \
    do                                                             \
    {                                                              \
        using namespace xiaoHttp::test;                            \
        FAIL(__VA_ARGS__);                                         \
        printTestStats();                                          \
        printErr() << "Force exiting due to a FAULT statement.\n"; \
        exit(1);                                                   \
    } while (0)

#define SUCCESS()                                                            \
    do                                                                       \
    {                                                                        \
        XIAOHTTP_TEST_START_SUPRESSION_                                      \
        XIAOHTTP_TEST_SUPPRESS_UNUSED_VALUE_WARNING_                         \
        TEST_CTX;                                                            \
        XIAOHTTP_TEST_END_SUPRESSION_                                        \
        if (xiaoHttp::test::internal::printSuccessfulTests)                  \
            xiaoHttp::test::print()                                          \
                << "\x1B[1;37mIn test case " << TEST_CTX->fullname() << "\n" \
                << "\x1B[0;37m↳ " << __FILE__ << ":" << __LINE__             \
                << " \x1B[0;32m PASSED:\x1B[0m\n"                            \
                << "  \033[0;34mSUCCESS()\x1B[0m\n\n";                       \
        xiaoHttp::test::internal::numAssertions++;                           \
        xiaoHttp::test::internal::numCorrectAssertions++;                    \
    } while (0)

#define XIAOHTTP_TEST_CLASS_NAME_(test_name) \
    XIAOHTTP_TEST_CONCAT(XIAOHTTP_TESTCASE_PREIX_, test_name)

#define XIAOHTTP_TEST(test_name)                                             \
    struct XIAOHTTP_TEST_CLASS_NAME_(test_name)                              \
        : public xiaoHttp::DrObject<XIAOHTTP_TEST_CLASS_NAME_(test_name)>,   \
          public xiaoHttp::test::TestCase                                    \
    {                                                                        \
        XIAOHTTP_TEST_CLASS_NAME_(test_name)                                 \
        () : xiaoHttp::test::TestCase(#test_name)                            \
        {                                                                    \
        }                                                                    \
        inline void doTest_(std::shared_ptr<xiaoHttp::test::Case>) override; \
    };                                                                       \
    void XIAOHTTP_TEST_CLASS_NAME_(test_name)::doTest_(                      \
        std::shared_ptr<xiaoHttp::test::Case> TEST_CTX)

#define SUBTEST(name) (std::make_shared<xiaoHttp::test::Case>(TEST_CTX, name))
#define SUBSECTION(name)                                                   \
    for (std::shared_ptr<xiaoHttp::test::Case> ctx_hold__ = TEST_CTX,      \
                                               ctx_tmp__ = SUBTEST(#name); \
         ctx_tmp__ != nullptr;                                             \
         TEST_CTX = ctx_hold__, ctx_tmp__ = nullptr)                       \
        if (TEST_CTX = ctx_tmp__, TEST_CTX != nullptr)
