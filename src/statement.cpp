#include "sqlw/statement.hpp"
#include "sqlw/cmake_vars.h"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/status.hpp"
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

sqlw::Statement::Statement(sqlw::Connection* con) :
    m_connection(con)
{}

sqlw::Statement::~Statement()
{
	sqlite3_finalize(m_stmt);
}

sqlw::Statement::Statement(sqlw::Statement&& other) noexcept
{
	*this = std::move(other);
}

sqlw::Statement& sqlw::Statement::operator=(sqlw::Statement&& other) noexcept
{
	if (this != &other)
	{
		m_status = other.m_status;
		m_stmt = other.m_stmt;
		m_connection = other.m_connection;
		m_unused_sql = other.m_unused_sql;

		other.m_stmt = nullptr;
		other.m_connection = nullptr;
		other.m_unused_sql = nullptr;
	}

	return *this;
}

sqlw::Statement& sqlw::Statement::exec(sqlw::Statement::callback_type callback)
{
	auto rc = sqlite3_step(m_stmt);
	m_status = static_cast<status::Code>(rc);

	if (!sqlw::status::is_ok(m_status))
	{
		return *this;
	}

	const auto col_count = sqlite3_data_count(m_stmt);

	if (0 == col_count)
	{
		m_status = status::Code::SQLW_DONE;
	}

	if (status::Code::SQLW_DONE == m_status)
	{
		return *this;
	}

	for (auto i = 0; i < col_count; i++)
	{
		const auto t = static_cast<sqlw::Type>(sqlite3_column_type(m_stmt, i));

		if (callback)
		{
			callback(
			    {col_count, sqlite3_column_name(m_stmt, i), t, column_value(t, i)}
			);
		}
	}

	return *this;
}

sqlw::Statement& sqlw::Statement::prepare(std::string_view sql)
{
	auto rc = sqlite3_prepare_v2(
	    m_connection->handle(),
	    sql.data(),
	    sql.size(),
	    &m_stmt,
	    &m_unused_sql
	);

	m_status = static_cast<status::Code>(rc);

	return *this;
}

void sqlw::Statement::operator()(sqlw::Statement::callback_type callback)
{
	size_t iter = 0;
	do
	{
		exec(callback);
		iter++;

		std::string_view unused {m_unused_sql};

		if (sqlw::status::Code::SQLW_DONE == m_status && !unused.empty())
		{
			sqlite3_finalize(m_stmt);
			prepare(unused);
		}
		else if (sqlw::status::Code::SQLW_ROW != m_status)
		{
			break;
		}
	}
	while (iter < SQLW_EXEC_LIMIT);
}

void sqlw::Statement::operator()(
    std::string_view sql,
    sqlw::Statement::callback_type callback
)
{
	prepare(sql);

	operator()(callback);
}

// @todo Определять динамически, когда использовать SQLITE_TRANSIENT,
// а когда SQLITE_STATIC.
sqlw::Statement& sqlw::Statement::bind(
    int idx,
    std::string_view value,
    sqlw::Type t
)
{
	int rc = 0;

	switch (t)
	{
		case sqlw::Type::SQL_TEXT:
			rc = sqlite3_bind_text(
			    m_stmt,
			    idx,
			    value.data(),
			    value.size(),
			    SQLITE_TRANSIENT
			);
			break;
		case sqlw::Type::SQL_DOUBLE:
			rc = sqlite3_bind_double(m_stmt, idx, std::stod(value.data()));
			break;
		case sqlw::Type::SQL_BLOB:
			rc = sqlite3_bind_blob(
			    m_stmt,
			    idx,
			    value.data(),
			    value.size(),
			    SQLITE_TRANSIENT
			);
			break;
		case sqlw::Type::SQL_INT:
			rc = sqlite3_bind_int(m_stmt, idx, std::stoi(value.data()));
			break;
		case sqlw::Type::SQL_NULL:
			rc = sqlite3_bind_null(m_stmt, idx);
			break;
	}

	m_status = static_cast<status::Code>(rc);

	return *this;
}

std::string sqlw::Statement::column_value(sqlw::Type type, int column_idx)
{
	switch (type)
	{
		case sqlw::Type::SQL_INT:
			{
				std::ostringstream ss;

				ss << sqlite3_column_int(m_stmt, column_idx);

				return ss.str();
			}
		case sqlw::Type::SQL_DOUBLE:
			{
				std::ostringstream ss;

				ss << sqlite3_column_double(m_stmt, column_idx);

				return ss.str();
			}
		case sqlw::Type::SQL_TEXT:
			{
				const std::string::size_type size =
				    sqlite3_column_bytes(m_stmt, column_idx);
				return {
				    reinterpret_cast<const char*>(
				        sqlite3_column_text(m_stmt, column_idx)
				    ),
				    size};
			}
		case sqlw::Type::SQL_BLOB:
			{
				const std::string::size_type size =
				    sqlite3_column_bytes(m_stmt, column_idx);
				return {
				    static_cast<const char*>(
				        sqlite3_column_blob(m_stmt, column_idx)
				    ),
				    size};
			}
		case sqlw::Type::SQL_NULL:
			return "";
		default:
			return "";
	}
}
