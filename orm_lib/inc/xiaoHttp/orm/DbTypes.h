/**
 * @file DbTypes.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-24
 *
 *
 */

#pragma once

namespace xiaoHttp
{
    namespace orm
    {
        class DefautlValue
        {
        };

        namespace internal
        {
            enum FieldType
            {
                MySqlTiny,
                MySqlShort,
                MySqlLong,
                MySqlLongLong,
                MySqlNull,
                MySqlString,
                XiaoHttpDefaultValue,
            };
        }
    }
}