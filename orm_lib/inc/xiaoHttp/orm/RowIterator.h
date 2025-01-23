/**
 * @file RowIterator.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 *
 */
#pragma once

#include <xiaoHttp/orm/Field.h>
#include <xiaoHttp/orm/Row.h>

namespace xiaoHttp
{
    namespace orm
    {
        class ConstRowIterator : protected Field
        {
        public:
            using pointer = const Field *;
            using reference = const Field &;
            using value_type = const Field;
            using size_type = Row::SizeType;
            using difference_type = Row::DifferenceType;
            using iterator_category = std::random_access_iterator_tag;

            pointer operator->()
            {
                return this;
            }

            reference operator*()
            {
                return *this;
            }

            ConstRowIterator operator++(int);

            ConstRowIterator &operator++()
            {
                ++column_;
                return *this;
            }

            ConstRowIterator operator--(int);

            ConstRowIterator &operator--()
            {
                --column_;
                return *this;
            }

            ConstRowIterator &operator+=(difference_type i)
            {
                column_ += i;
                return *this;
            }

            ConstRowIterator &operator-=(difference_type i)
            {
                column_ -= i;
                return *this;
            }

            bool operator==(const ConstRowIterator &other) const
            {
                return column_ == other.column_;
            }

            bool operator!=(const ConstRowIterator &other) const
            {
                return column_ != other.column_;
            }

            bool operator>(const ConstRowIterator &other) const
            {
                return column_ > other.column_;
            }

            bool operator<(const ConstRowIterator &other) const
            {
                return column_ < other.column_;
            }

            bool operator>=(const ConstRowIterator &other) const
            {
                return column_ >= other.column_;
            }

            bool operator<=(const ConstRowIterator &other) const
            {
                return column_ <= other.column_;
            }

        private:
            friend class Row;

            ConstRowIterator(const Row &r, SizeType column) noexcept : Field(r, column)
            {
            }
        };

        class ConstReverseRowIterator : private ConstRowIterator
        {
        public:
            using super = ConstRowIterator;
            using iterator_type = ConstRowIterator;
            using iterator_type::difference_type;
            using iterator_type::iterator_category;
            using iterator_type::pointer;
            using iterator_type::reference;

            ConstReverseRowIterator(const ConstReverseRowIterator &rhs)
                : ConstRowIterator(rhs)
            {
            }

            explicit ConstReverseRowIterator(const ConstRowIterator &rhs)
                : ConstRowIterator(rhs)
            {
            }

            ConstRowIterator base() const noexcept;

            using iterator_type::operator->;
            using iterator_type::operator*;

            ConstReverseRowIterator operator++(int);

            ConstReverseRowIterator &operator++()
            {
                iterator_type::operator--();
                return *this;
            }

            ConstReverseRowIterator operator--(int);

            ConstReverseRowIterator &operator--()
            {
                iterator_type::operator++();
                return *this;
            }

            ConstReverseRowIterator &operator+=(difference_type i)
            {
                iterator_type::operator-=(i);
                return *this;
            }

            ConstReverseRowIterator &operator-=(difference_type i)
            {
                iterator_type::operator+=(i);
                return *this;
            }

            bool operator==(const ConstReverseRowIterator &other) const
            {
                return column_ == other.column_;
            }

            bool operator!=(const ConstReverseRowIterator &other) const
            {
                return column_ != other.column_;
            }

            bool operator>(const ConstReverseRowIterator &other) const
            {
                return column_ < other.column_;
            }

            bool operator<(const ConstReverseRowIterator &other) const
            {
                return column_ > other.column_;
            }

            bool operator>=(const ConstReverseRowIterator &other) const
            {
                return column_ <= other.column_;
            }

            bool operator<=(const ConstReverseRowIterator &other) const
            {
                return column_ >= other.column_;
            }
        };
    }
}