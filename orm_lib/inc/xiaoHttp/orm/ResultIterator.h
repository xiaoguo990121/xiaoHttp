/**
 * @file ResultIterator.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 *
 */

#pragma once

#include <xiaoHttp/orm/Result.h>
#include <xiaoHttp/orm/Row.h>

namespace xiaoHttp
{
    namespace orm
    {
        class ConstResultIterator : protected Row
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using pointer = const Row *;
            using reference = const Row &;
            using value_type = const Row;
            using size_type = Result::SizeType;
            using difference_type = Result::DifferenceType;

            pointer operator->()
            {
                return this;
            }

            reference operator*()
            {
                return *this;
            }

            ConstResultIterator operator++(int);

            ConstResultIterator &operator++()
            {
                ++index_;
                return *this;
            }

            ConstResultIterator operator--(int);

            ConstResultIterator &operator--()
            {
                --index_;
                return *this;
            }

            ConstResultIterator &operator+=(difference_type i)
            {
                index_ += i;
                return *this;
            }

            ConstResultIterator &operator-=(difference_type i)
            {
                index_ -= i;
                return *this;
            }

            bool operator==(const ConstResultIterator &other) const
            {
                return index_ == other.index_;
            }

            bool operator!=(const ConstResultIterator &other) const
            {
                return index_ != other.index_;
            }

            bool operator>(const ConstResultIterator &other) const
            {
                return index_ > other.index_;
            }

            bool operator<(const ConstResultIterator &other) const
            {
                return index_ < other.index_;
            }

            bool operator>=(const ConstResultIterator &other) const
            {
                return index_ >= other.index_;
            }

            bool operator<=(const ConstResultIterator &other) const
            {
                return index_ <= other.index_;
            }

            ConstResultIterator(const ConstResultIterator &) noexcept = default;
            ConstResultIterator(ConstResultIterator &&) noexcept = default;

        private:
            friend class Result;

            ConstResultIterator(const Result &r, SizeType index) noexcept
                : Row(r, index)
            {
            }
        };

        class ConstReverseResultIterator : private ConstResultIterator
        {
        public:
            using super = ConstResultIterator;
            using iterator_type = ConstResultIterator;
            using iterator_type::difference_type;
            using iterator_type::iterator_category;
            using iterator_type::pointer;
            using iterator_type::reference;

            // using iterator_type::value_type;

            ConstReverseResultIterator(const ConstReverseResultIterator &rhs)
                : ConstResultIterator(rhs)
            {
            }

            explicit ConstReverseResultIterator(const ConstResultIterator &rhs)
                : ConstResultIterator(rhs)
            {
                super::operator--();
            }

            ConstResultIterator base() const noexcept;

            using iterator_type::operator->;
            using iterator_type::operator*;

            ConstReverseResultIterator operator++(int);

            ConstReverseResultIterator &operator++()
            {
                iterator_type::operator--();
                return *this;
            }

            ConstReverseResultIterator operator--(int);

            ConstReverseResultIterator &operator--()
            {
                iterator_type::operator++();
                return *this;
            }

            ConstReverseResultIterator &operator+=(difference_type i)
            {
                iterator_type::operator-=(i);
                return *this;
            }

            ConstReverseResultIterator &operator-=(difference_type i)
            {
                iterator_type::operator+=(i);
                return *this;
            }

            bool operator==(const ConstReverseResultIterator &other) const
            {
                return index_ == other.index_;
            }

            bool operator!=(const ConstReverseResultIterator &other) const
            {
                return index_ != other.index_;
            }

            bool operator>(const ConstReverseResultIterator &other) const
            {
                return index_ < other.index_;
            }

            bool operator<(const ConstReverseResultIterator &other) const
            {
                return index_ > other.index_;
            }

            bool operator>=(const ConstReverseResultIterator &other) const
            {
                return index_ <= other.index_;
            }

            bool operator<=(const ConstReverseResultIterator &other) const
            {
                return index_ >= other.index_;
            }
        };

    }
}