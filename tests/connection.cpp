#include "sqlw/connection.hpp"
#include "sqlw/cmake_vars.h"
#include "sqlw/forward.hpp"
#include "sqlw/status.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

TEST(Connection, can_create_new_db_file_on_ctor)
{
	auto path = std::filesystem::temp_directory_path() / "test.db";

	sqlw::Connection db_con {path.string()};

	ASSERT_TRUE(std::filesystem::exists(path));
	ASSERT_EQ(sqlw::status::Code::OK, db_con.status())
	    << sqlw::status::verbose(db_con.status());

	std::remove(path.string().data());
}

TEST(Connection, can_open_existing_db_file_on_ctor)
{
	auto path = std::filesystem::temp_directory_path() / "test_old.db";

	std::ofstream db_file {path};

	ASSERT_TRUE(std::filesystem::exists(path));

	sqlw::Connection db_con {path.string()};

	ASSERT_TRUE(std::filesystem::exists(path));
	ASSERT_EQ(sqlw::status::Code::OK, db_con.status())
	    << sqlw::status::verbose(db_con.status());

	std::remove(path.string().data());
}

TEST(Connection, can_open_in_memory_db)
{
	sqlw::Connection db_con {":memory:"};

	ASSERT_FALSE(std::filesystem::exists(":memory:"));
	ASSERT_EQ(sqlw::status::Code::OK, db_con.status())
	    << sqlw::status::verbose(db_con.status());
}
