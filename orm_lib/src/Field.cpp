/**
 * @file Field.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 *
 */

#include <xiaoHttp/orm/Field.h>
#include <xiaoHttp/utils/Utilities.h>
#include <xiaoLog/Logger.h>
#include <stdlib.h>

using namespace xiaoHttp::orm;

Field::Field(const Row &row, Row::SizeType columnNum) noexcept
    : row_(Result::SizeType(row, index_)),
      column_((long)columnNum),
      result_(row.result_)
{
}

const char *Field::name() const
{
    return result_.columnName(column_);
}

bool Field::isNull() const
{
    return result_.isNull(row_, column_);
}

template <>
std::string Field::as<std::string>() const
{
    if (result_.oid(column_) != 17)
    {
        auto data_ = result_.getValue(row_, column_);
        auto dataLength_ = result_.getLength(row_, column_);
        return std::string(data_, dataLength_);
    }
    else
    {
        auto sv = as<std::string_view>();
        if (sv.length() < 2 || sv[0] != '\\' || sv[1] != 'x')
            return std::string();
        return utils::hexToBinaryString(sv.data() + 2, sv.length() - 2);
    }
}

template <>
const char *Field::as<const char *>() const
{
    auto data_ = result_.getValue(row_, column_);
    return data_;
}

template <>
char *Field::as<char *>() const
{
    auto data_ = result_.getValue(row_, column_);
    return (char *)data_;
}

template <>
std::vector<char> Field::as<std::vector<char>>() const
{
    if (result_.oid(column_) != 17)
    {
        char *first = (char *)result_.getValue(row_, column_);
        char *last = first + result_.getLength(row_, column_);
        return std::vector<char>(first, last);
    }
    else
    {
        // Bytea type of PostgreSQL
        auto sv = as<std::string_view>();
        if (sv.length() < 2 || sv[0] != '\\' || sv[1] != 'x')
            return std::vector<char>();
        return utils::hexToBinaryVector(sv.data() + 2, sv.length() - 2);
    }
}

const char *Field::c_str() const
{
    return as<const char *>();
}