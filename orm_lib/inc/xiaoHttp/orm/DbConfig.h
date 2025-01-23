/**
 * @file DbConfig.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-23
 *
 *
 */

#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace xiaoHttp::orm
{
    struct PostgresConfig
    {
        std::string host;
        unsigned short port;
        std::string databaseName;
        std::string username;
        std::string password;
        size_t connectionNumber;
        std::string name;
        bool isFast;
        std::string characterSet;
        double timeout;
        bool autoBatch;
        std::unordered_map<std::string, std::string> connectOptions;
    };

    struct MysqlConfig
    {
        std::string host;
        unsigned short port;
        std::string databaseName;
        std::string username;
        std::string password;
        size_t connectionNumber;
        std::string name;
        bool isFast;
        std::string characterSet;
        double timeout;
    };

    struct Sqlite3Config
    {
        size_t connectionNumber;
        std::string filename;
        std::string name;
        double timeout;
    };

    using DbConfig = std::variant<PostgresConfig, MysqlConfig, Sqlite3Config>;

}