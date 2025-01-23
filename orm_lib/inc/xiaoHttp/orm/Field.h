/**
 * @file Field.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <string_view>
#include <xiaoHttp/orm/ArrayParser.h>
#include <xiaoHttp/orm/Result.h>
#include <xiaoHttp/orm/Row.h>
#include <xiaoLog/Logger.h>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#ifdef __linux__
#include <arpa/inet.h>
#endif

namespace xiaoHttp
{
    namespace orm
    {
        /// Reference to a field in a result set.
        /**
         * A field represents one entry in a row.  It represents an actual value
         * in the result set, and can be converted to various types.
         */
        class XIAOHTTP_EXPORT Field
        {
        public:
            using SizeType = unsigned long;

            const char *name() const;

            bool isNull() const;

            const char *c_str() const;

            size_t length() const
            {
                return result_.getLength(row_, column_);
            }

            template <typename T>
            T as() const
            {
                if (isNull())
                    return T();
            }

        protected:
            Result::SizeType row_;
            /**
             * Column number
             * You'd expect this to be a size_t, but due to the way reverse iterators
             * are related to regular iterators, it must be allowed to underflow to -1.
             */
            long column_;
            friend class Row;
            Field(const Row &row, Row::SizeType columnNum) noexcept;

        private:
            const Result result_;
        };
    }
}