/**
 * @file Result.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <memory>

namespace xiaoHttp
{
    namespace orm
    {
        class ConstResultIterator;
        class ConstReverseResultIterator;
        class Row;
        class ResultImpl;
        using ResultImplPtr = std::shared_ptr<ResultImpl>;

        enum class SqlStatus
        {
            Ok,
            End
        };

        class XIAOHTTP_EXPORT Result
        {
        public:
            explicit Result(ResultImplPtr ptr) : resultPtr_(std::move(ptr))
            {
            }

            Result(const)

                private : ResultImplPtr resultPtr_;

            friend class Field;
            friend class Row;

            RowSizeType
        };
    }

}
