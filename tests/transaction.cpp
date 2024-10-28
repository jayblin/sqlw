#include "sqlw/transaction.hpp"
#include "sqlw/statement.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <system_error>
#include <tuple>
#include <utility>

static void errorLogCallback(void* pArg, int iErrCode, const char* zMsg)
{
    std::cout << '[' << iErrCode << "] " << zMsg << '\n';
}

GTEST_TEST(Transaction, does_persist_data)
{
    sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

    sqlw::Connection con{":memory:"};
    sqlw::Statement stmt{&con};

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    stmt(R"sql(
    CREATE TABLE user (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE
    )
    )sql");
    ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    sqlw::Transaction transaction{&con};

    ec = transaction(
        "INSERT INTO user (id, name) VALUES (?1, ?2)",
        std::tuple{2, std::pair("Boba", sqlw::Type::SQL_TEXT)});
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    std::stringstream ss;
    stmt("SELECT * FROM user", [&](sqlw::Statement::ExecArgs e) {
        ss << e.column_value << ",";
    });

    ASSERT_STREQ("2,Boba,", ss.str().data());
}

GTEST_TEST(Transaction, does_rollback)
{
    sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

    sqlw::Connection con{":memory:"};
    sqlw::Statement stmt{&con};

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    stmt(R"sql(
    CREATE TABLE user (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE
    )
    )sql");
    ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    stmt("INSERT INTO user (id, name) VALUES (1, 'Sonne')");
    ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    sqlw::Transaction transaction{&con};

    ec = transaction(
        "INSERT INTO user (id, name) VALUES (?1, ?2);"
        "SELECT * FROM nonexistent",
        std::tuple{2, std::pair("Boba", sqlw::Type::SQL_TEXT)});
    ASSERT_TRUE(sqlw::status::Condition::OK != ec) << ec.message();

    std::stringstream ss;
    stmt("SELECT * FROM user", [&](sqlw::Statement::ExecArgs e) {
        ss << e.column_value << ",";
    });

    ASSERT_STREQ("1,Sonne,", ss.str().data());
}

GTEST_TEST(Transaction, can_callback)
{
    sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

    sqlw::Connection con{":memory:"};
    sqlw::Statement stmt{&con};

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    stmt(R"sql(
    CREATE TABLE user (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE
    )
    )sql");
    ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    sqlw::Transaction transaction{&con};

    std::stringstream ss;
    ec = transaction(
        "INSERT INTO user (id, name) VALUES (?1, ?2);SELECT * FROM user",

        [&ss](sqlw::Statement::ExecArgs e) { ss << e.column_value << ","; },
        std::tuple{
            2, std::pair(std::string_view{"Boba"}, sqlw::Type::SQL_TEXT)});
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();
    ASSERT_STREQ("2,Boba,", ss.str().data());
}

GTEST_TEST(Transaction, can_execute_without_bindings)
{
    sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

    sqlw::Connection con{":memory:"};
    sqlw::Statement stmt{&con};

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    stmt(R"sql(
    CREATE TABLE user (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL UNIQUE
    )
    )sql");
    ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    sqlw::Transaction t{&con};

    ec = t("INSERT INTO user (id, name) VALUES (12, 'John Doe')");
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec.message();

    std::stringstream ss;
    stmt("SELECT * FROM user", [&](sqlw::Statement::ExecArgs e) {
        ss << e.column_value << ",";
    });

    ASSERT_STREQ("12,John Doe,", ss.str().data());
}
