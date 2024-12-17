#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

TEST(Connection, can_create_new_db_file_on_ctor)
{
    auto path = std::filesystem::temp_directory_path() / "test.db";

    sqlw::Connection db_con{path.string()};

    ASSERT_TRUE(std::filesystem::exists(path));

    std::error_code ec = db_con.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;

    std::remove(path.string().data());
}

TEST(Connection, can_open_existing_db_file_on_ctor)
{
    auto path = std::filesystem::temp_directory_path() / "test_old.db";

    std::ofstream db_file{path};

    ASSERT_TRUE(std::filesystem::exists(path));

    sqlw::Connection db_con{path.string()};

    ASSERT_TRUE(std::filesystem::exists(path));

    std::error_code ec = db_con.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;

    std::remove(path.string().data());
}

TEST(Connection, can_open_in_memory_db)
{
    sqlw::Connection db_con{":memory:"};

    ASSERT_FALSE(std::filesystem::exists(":memory:"));

    std::error_code ec = db_con.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;
}
