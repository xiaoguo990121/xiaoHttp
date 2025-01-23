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
                auto data_ = result_.getValue(row_, column_);
                T value = T();
                if (data_)
                {
                    try
                    {
                        std::stringstream ss(data_);
                        ss >> value;
                    }
                    catch (...)
                    {
                        LOG_DEBUG << "Type error";
                    }
                }
                return value;
            }

            /// Parse the field as an SQL array.
            /**
             * Call the parser to retrieve values (and structure) from the array.
             *
             * Make sure the @c result object stays alive until parsing is finished.  If
             * you keep the @c row of @c field object alive, it will keep the @c result
             * object alive as well.
             */
            ArrayParser getArrayParser() const
            {
                return ArrayParser(result_.getValue(row_, column_));
            }

            template <typename T>
            std::vector<std::shared_ptr<T>> asArray() const
            {
                std::vector<std::shared_ptr<T>> ret;
                auto arrParser = getArrayParser();
                while (1)
                {
                    auto arrVal = arrParser.getNext();
                    if (arrVal.first == ArrayParser::juncture::done)
                    {
                        break;
                    }
                    if (arrVal.first == ArrayParser::juncture::string_value)
                    {
                        T val;
                        std::stringstream ss(std::move(arrVal.second));
                        ss >> val;
                        ret.push_back(std::shared_ptr<T>(new T(val)));
                    }
                    else if (arrVal.first == ArrayParser::juncture::null_value)
                    {
                        ret.push_back(std::shared_ptr<T>());
                    }
                }
                return ret;
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

        template <>
        XIAOHTTP_EXPORT std::string Field::as<std::string>() const;
        template <>
        XIAOHTTP_EXPORT const char *Field::as<const char *>() const;
        template <>
        XIAOHTTP_EXPORT char *Field::as<char *>() const;
        template <>
        XIAOHTTP_EXPORT std::vector<char> Field::as<std::vector<char>>() const;

        template <>
        inline std::string_view Field::as<std::string_view>() const
        {
            auto first = result_.getValue(row_, column_);
            auto length = result_.getLength(row_, column_);
            return {first, length};
        }

        template <>
        inline float Field::as<float>() const
        {
            if (isNull())
                return 0.0;
            return std::stof(result_.getValue(row_, column_));
        }

        template <>
        inline double Field::as<double>() const
        {
            if (isNull())
                return 0.0;
            return std::stod(result_.getValue(row_, column_));
        }

        template <>
        inline bool Field::as<bool>() const
        {
            if (result_.getLength(row_, column_) != 1)
            {
                return false;
            }
            auto value = result_.getValue(row_, column_);
            if (*value == 't' || *value == '1')
                return true;
            return false;
        }

        emplate<> inline int Field::as<int>() const
        {
            if (isNull())
                return 0;
            return std::stoi(result_.getValue(row_, column_));
        }

        template <>
        inline long Field::as<long>() const
        {
            if (isNull())
                return 0;
            return std::stol(result_.getValue(row_, column_));
        }

        template <>
        inline int8_t Field::as<int8_t>() const
        {
            if (isNull())
                return 0;
            return static_cast<int8_t>(atoi(result_.getValue(row_, column_)));
        }

        template <>
        inline long long Field::as<long long>() const
        {
            if (isNull())
                return 0;
            return atoll(result_.getValue(row_, column_));
        }

        template <>
        inline unsigned int Field::as<unsigned int>() const
        {
            if (isNull())
                return 0;
            return static_cast<unsigned int>(
                std::stoul(result_.getValue(row_, column_)));
        }

        template <>
        inline unsigned long Field::as<unsigned long>() const
        {
            if (isNull())
                return 0;
            return std::stoul(result_.getValue(row_, column_));
        }

        template <>
        inline uint8_t Field::as<uint8_t>() const
        {
            if (isNull())
                return 0;
            return static_cast<uint8_t>(atoi(result_.getValue(row_, column_)));
        }

        template <>
        inline unsigned long long Field::as<unsigned long long>() const
        {
            if (isNull())
                return 0;
            return std::stoull(result_.getValue(row_, column_));
        }
    }
}