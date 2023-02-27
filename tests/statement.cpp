#include "sqlw/statement.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/status.hpp"
#include <gtest/gtest.h>
#include <sstream>

TEST(Statement, can_execute_statements)
{
	sqlw::Connection con {":memory:"};

	{
		sqlw::Statement stmt {
			&con,
			R"(
				CREATE TABLE user (
					id INTEGER PRIMARY KEY AUTOINCREMENT,
					name TEXT NOT NULL UNIQUE
				);
			)"
		};

		ASSERT_EQ(sqlw::status::Code::OK, stmt.status());

		stmt.exec();

		ASSERT_EQ(sqlw::status::Code::DONE, stmt.status());
	}

	{
		sqlw::Statement stmt {
			&con,
			R"(
				INSERT INTO user (id, name) VALUES (1,'kate'),(2,'eris');
			)"
		};

		ASSERT_EQ(sqlw::status::Code::OK, stmt.status());

		stmt.exec();

		ASSERT_EQ(sqlw::status::Code::DONE, stmt.status());
	}

	{
		sqlw::Statement stmt {
			&con,
			"SELECT * FROM user WHERE id <> 1"
		};

		ASSERT_EQ(sqlw::status::Code::OK, stmt.status());

		std::stringstream ss;
		int i = 0;

		auto callback = [&i, &ss](sqlw::Statement::ExecArgs args)
		{
			i++;

			ss << args.column_name << ":" << args.column_value;

			if (i == args.column_count)
			{
				ss << '\n';
				i = 0;
			}
			else if (i < args.column_count)
			{
				ss << ',';
			}
		};

		stmt.exec(callback);

		ASSERT_PRED2(
			[] (std::string expected, std::string actual)
			{
				return expected == actual;
			},
			"id:2,name:eris\n",
			ss.str()
		);
	}

	{
		sqlw::Statement stmt {
			&con,
			"SELECT * FROM user"
		};

		ASSERT_EQ(sqlw::status::Code::OK, stmt.status());

		std::stringstream ss;
		int i = 0;

		auto callback = [&i, &ss](sqlw::Statement::ExecArgs args)
		{
			i++;

			ss << args.column_name << ":" << args.column_value;

			if (i == args.column_count)
			{
				ss << '\n';
				i = 0;
			}
			else if (i < args.column_count)
			{
				ss << ',';
			}
		};

		stmt.exec_until_done(callback);

		ASSERT_PRED2(
			[] (std::string expected, std::string actual)
			{
				return expected == actual;
			},
			"id:1,name:kate\nid:2,name:eris\n",
			ss.str()
		);
	}
}
