#include "sqlw/json_string_result.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/statement.hpp"
#include "sqlw/status.hpp"
#include <gtest/gtest.h>

TEST(JsonStringResult, can_give_json_result)
{
    sqlw::Connection con{":memory:"};
    sqlw::Statement stmt{&con};

    stmt(R"(CREATE TABLE user (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT NOT NULL UNIQUE
	))");

    stmt("INSERT INTO user (id, name) VALUES (1,'john'),(2,'bob')");

    ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
        << sqlw::status::verbose(stmt.status());

    {
        auto jsr =
            stmt.operator()<sqlw::JsonStringResult>("SELECT * FROM user");

        ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
            << sqlw::status::verbose(stmt.status());
        ASSERT_EQ(
            R"([{"id":1,"name":"john"},{"id":2,"name":"bob"}])",
            jsr.get_array_result());
    }

    {
        auto jsr = stmt.operator()<sqlw::JsonStringResult>(
            "SELECT * FROM user WHERE id = 2 LIMIT 1");

        ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
            << sqlw::status::verbose(stmt.status());
        ASSERT_EQ(R"({"id":2,"name":"bob"})", jsr.get_object_result());
    }
}
