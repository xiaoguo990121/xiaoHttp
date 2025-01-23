/**
 * @file Row.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-23
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <xiaoHttp/orm/Result.h>
#include <string>

namespace xiaoHttp
{
    namespace orm
    {
        class Field;
        class ConstRowIterator;
        class ConstReverseRowIterator;

        class XIAOHTTP_EXPORT Row
        {
        public:
            using SizeType = size_t;
            using Reference = Field;
            using ConstIterator = ConstRowIterator;
            using Iterator = ConstIterator;
            using ConstReverseIterator = ConstReverseRowIterator;

            using DifferenceType = long;

            Reference operator[](SizeType index) const noexcept;
            Reference operator[](int index) const noexcept;
            Reference operator[](const char columnName[]) const;
            Reference operator[](const std::string &columnName) const;

            Reference at(SizeType index) const;
            Reference at(const char columnName[]) const;
            Reference at(const std::string &columnName) const;

            SizeType size() const;

            SizeType capacity() const noexcept
            {
                return size();
            }

            ConstIterator begin() const noexcept;
            ConstIterator cbegin() const noexcept;
            ConstIterator end() const noexcept;
            ConstIterator cend() const noexcept;

            ConstReverseIterator rbegin() const;
            ConstReverseIterator crbegin() const;
            ConstReverseIterator rend() const;
            ConstReverseIterator crend() const;

#ifdef _MSC_VER
            Row() noexcept = default;
#endif

            Row(const Row &r) noexcept = default;
            Row(Row &&) noexcept = default;
            Row &operator=(const Row &) noexcept = default;

        private:
            Result result_;

        protected:
            friend class Field;
            /**
             * Row number
             * You'd expect this to be a size_t, but due to the way reverse iterators
             * are related to regular iterators, it must be allowed to underflow to -1.
             */
            long index_{0};
            Row::SizeType end_{0};
            friend class Result;
            Row(const Result &r, SizeType index) noexcept;
        };
    }
}