#include "sqlw/statement.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/status.hpp"
#include <gtest/gtest.h>
#include <sstream>

TEST(Statement, can_execute_statements)
{
	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

	ASSERT_EQ(sqlw::status::Code::SQLW_OK, stmt.status())
	    << sqlw::status::verbose(stmt.status());

	stmt(R"(CREATE TABLE user (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT NOT NULL UNIQUE
	);)");

	ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
	    << sqlw::status::verbose(stmt.status());

	stmt("INSERT INTO user (id, name) VALUES (1,'kate'),(2,'eris');");

	ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
	    << sqlw::status::verbose(stmt.status());

	{
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

		stmt("SELECT * FROM user WHERE id <> 1", callback);

		ASSERT_PRED2(
		    [](std::string expected, std::string actual)
		    {
			    return expected == actual;
		    },
		    "id:2,name:eris\n",
		    ss.str()
		) << sqlw::status::verbose(stmt.status());
	}

	{
		stmt("SELECT * FROM user");

		ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
		    << sqlw::status::verbose(stmt.status());

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

		stmt("SELECT * FROM user", callback);

		ASSERT_PRED2(
		    [](std::string expected, std::string actual)
		    {
			    return expected == actual;
		    },
		    "id:1,name:kate\nid:2,name:eris\n",
		    ss.str()
		) << sqlw::status::verbose(stmt.status());
	}
}

TEST(Statement, can_execute_multiple_statements)
{
	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

	stmt(
	    R"(CREATE TABLE user (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name TEXT NOT NULL UNIQUE
		);)"
	    "INSERT INTO user (id, name) VALUES (1,'zavala'),(2,'cade');"
	);

	ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
	    << sqlw::status::verbose(stmt.status());

	{
		std::stringstream ss;
		int i = 0;

		stmt(
		    "SELECT * FROM user",
		    [&i, &ss](sqlw::Statement::ExecArgs args)
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
		    }
		);

		ASSERT_PRED2(
		    [](std::string expected, std::string actual)
		    {
			    return expected == actual;
		    },
		    "id:1,name:zavala\nid:2,name:cade\n",
		    ss.str()
		) << sqlw::status::verbose(stmt.status());
	}
}

TEST(Statement, can_bind_parameters)
{
	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

	stmt(
	    R"(CREATE TABLE user (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name TEXT NOT NULL UNIQUE
		);)"
	    "INSERT INTO user (id, name) VALUES (1,'drifter'),(2,'clovis');"
	);

	ASSERT_EQ(sqlw::status::Code::SQLW_DONE, stmt.status())
	    << sqlw::status::verbose(stmt.status());

	{
		std::stringstream ss;
		int i = 0;

		stmt.prepare(
		        "SELECT * FROM user u WHERE u.name = ? OR u.id = ? ORDER BY u.id ASC"
		)
		    .bind(1, "clovis", sqlw::Type::SQL_TEXT)
		    .bind(2, "1", sqlw::Type::SQL_INT);

		ASSERT_EQ(sqlw::status::Code::SQLW_OK, stmt.status())
		    << sqlw::status::verbose(stmt.status());

		stmt(
		    [&i, &ss](sqlw::Statement::ExecArgs args)
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
		    }
		);

		ASSERT_PRED2(
		    [](std::string expected, std::string actual)
		    {
			    return expected == actual;
		    },
		    "id:1,name:drifter\nid:2,name:clovis\n",
		    ss.str()
		) << sqlw::status::verbose(stmt.status());
	}
}
