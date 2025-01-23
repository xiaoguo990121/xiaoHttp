/**
 * @file ArrayParser.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-23
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <stdexcept>
#include <string>
#include <utility>

namespace xiaoHttp
{
    namespace orm
    {
        /// Low-level array parser.
        /** Use this to read an array field retrieved from the database.
         *
         * Use this only if your client encoding is UTF-8, ASCII, or a single-byte
         * encoding which is a superset of ASCII.
         *
         * The input is a C-style string containing the textual representation of an
         * array, as returned by the database.  The parser reads this representation
         * on the fly.  The string must remain in memory until parsing is done.
         *
         * Parse the array by making calls to @c get_next until it returns a
         * @c juncture of "done".  The @c juncture tells you what the parser found in
         * that step: did the array "nest" to a deeper level, or "un-nest" back up?
         */
        class XIAOHTTP_EXPORT ArrayParser
        {
            /// What's the latest thing found in the array?
            enum juncture
            {
                /// Starting a new row
                row_start,
                /// Ending the current row.
                row_end,
                /// Found a NULL value.
                null_value,
                /// Found a string value.
                string_value,
                /// Parsing has completed.
                done,
            };

            explicit ArrayParser(const char input[]);

            std::pair<juncture, std::string> getNext();

        private:
            const char *pos_;
        };
    }
}