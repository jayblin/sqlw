#include "sqlw/statement.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <system_error>

static void errorLogCallback(void *pArg, int iErrCode, const char *zMsg)
{
	std::cout << '[' << iErrCode << "] " << zMsg << '\n';
}

GTEST_TEST(Statement, can_execute_statements)
{
	sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;

	stmt(R"(CREATE TABLE user (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT NOT NULL UNIQUE
	);)");

    ec = stmt.status();
	ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

	stmt("INSERT INTO user (id, name) VALUES (1,'kate'),(2,'eris');");

    ec = stmt.status();
	ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

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
		) << std::error_code{stmt.status()};
	}

	{
		stmt("SELECT * FROM user");

        ec = stmt.status();
        ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

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
		) << std::error_code{stmt.status()};
	}
}

GTEST_TEST(Statement, can_execute_multiple_statements)
{
	sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

	stmt(
	    R"(CREATE TABLE user (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name TEXT NOT NULL UNIQUE
		);)"
	    "INSERT INTO user (id, name) VALUES (1,'zavala'),(2,'cade');"
	);

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

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
		) << std::error_code{stmt.status()};
	}
}

GTEST_TEST(Statement, can_bind_parameters)
{
	sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

	stmt(
	    R"(CREATE TABLE user (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name TEXT NOT NULL UNIQUE
		);)"
	    "INSERT INTO user (id, name) VALUES (1,'drifter'),(2,'clovis');"
	);

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

	{
		std::stringstream ss;
		int i = 0;

		stmt.prepare(
		        "SELECT * FROM user u WHERE u.name = ? OR u.id = ? ORDER BY u.id ASC"
		)
		    .bind(1, "clovis", sqlw::Type::SQL_TEXT)
		    .bind(2, "1", sqlw::Type::SQL_INT);


        ec = stmt.status();
        ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;

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
		) << std::error_code{stmt.status()};
	}
}

GTEST_TEST(Statement, does_report_misuse)
{
	sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;

	stmt(R"(CREATE TABLE user (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT NOT NULL UNIQUE
	);)");


    ec = stmt.status();
	ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

	stmt("INSERT INTO user (id, name) VALUES (1,'kate'),(2,'eris'");

    ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::ERROR == ec) << ec;
}

GTEST_TEST(Statement, can_bind_double_type_parameters)
{
	sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

	sqlw::Connection con {":memory:"};
	sqlw::Statement stmt {&con};

	stmt(
	    R"(CREATE TABLE user (
			id INTEGER PRIMARY KEY AUTOINCREMENT,
			name REAL
			)
		)"
	);

    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

	stmt
		.prepare(
			"INSERT INTO user (id, name) "
			"VALUES "
			"(1,:1),"
			"(2,:2),"
			"(3,:3),"
			"(4,:4)"
		)
		.bind(1, "0.22", sqlw::Type::SQL_DOUBLE)
		.bind(2, 1.45)
		.bind(3, "100", sqlw::Type::SQL_DOUBLE)
		.bind(4, (double) 100)
		.exec();

    ec = stmt.status();
	ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;

	std::stringstream ss;
	stmt(
		"SELECT * FROM user",
		[&ss](sqlw::Statement::ExecArgs e)
		{
			ss << e.column_name << ':' << e.column_value << '\n';
		}
	);

	ASSERT_STREQ(
		"id:1\n"
		"name:0.22\n"
		"id:2\n"
		"name:1.45\n"
		"id:3\n"
		"name:100\n"
		"id:4\n"
		"name:100\n",
		ss.str().data()
	);
}
