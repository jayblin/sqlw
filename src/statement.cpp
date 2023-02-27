#include "sqlw/statement.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

/* sqlw::Statement::Statement() */
/* {} */

/* sqlw::Statement::Statement(const Sqlite3& db, const std::string& sql) */
/* { */
/* 	auto rc = sqlite3_prepare_v2( */
/* 		db.m_db, sql.data(), sql.size(), &m_stmt, nullptr */
/* 	); */
	
/* 	m_status = static_cast<status::Code>(rc); */
/* } */

sqlw::Statement::Statement(sqlw::Connection* con, std::string_view sql)
{
	auto rc = sqlite3_prepare_v2(
		con->handle(), sql.data(), sql.size(), &m_stmt, nullptr
	);
	
	m_status = static_cast<status::Code>(rc);
}

sqlw::Statement::~Statement()
{
	if (m_stmt)
	{
		sqlite3_finalize(m_stmt);
	}
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

		other.m_stmt = nullptr;
		other.m_connection = nullptr;
	}

	return *this;
}

sqlw::Statement& sqlw::Statement::exec()
{
	auto rc = sqlite3_step(m_stmt);
	m_status = static_cast<status::Code>(rc);

	return *this;
}

sqlw::Statement& sqlw::Statement::exec(
	std::function<void (ExecArgs)> callback
)
{
	exec();

	const auto col_count = sqlite3_data_count(m_stmt);

	if (0 == col_count)
	{
		m_status = status::Code::DONE;
	}

	if (status::Code::DONE == m_status)
	{
		return *this;
	}

	for (auto i = 0; i < col_count; i++)
	{
		const auto t = static_cast<sqlw::Type>(sqlite3_column_type(m_stmt, i));
		callback({
			col_count,
			sqlite3_column_name(m_stmt, i),
			t,
			column_value(t, i)
		});
	}

	return *this;
}


sqlw::Statement& sqlw::Statement::exec_until_done(
	std::function<void (sqlw::Statement::ExecArgs)> callback
)
{
	int _i = 0;
	do
	{
		++_i;
		exec(callback);

		if (status::Code::DONE == m_status)
		{
			break;
		}
	}
	/** @todo maybe let user decide maximum iteration count via compile flag?*/
	while (status::Code::ROW == m_status  && _i < 256);

	return *this;
}

// @todo Определять динамически, когда использовать SQLITE_TRANSIENT,
// а когда SQLITE_STATIC.
sqlw::Statement& sqlw::Statement::bind(int idx, std::string_view value, sqlw::Type t)
{
	int rc = 0;

	switch (t)
	{
		case sqlw::Type::SQL_TEXT:
			rc = sqlite3_bind_text(
				m_stmt, idx, value.data(), value.size(), SQLITE_TRANSIENT
			);
			break;
		case sqlw::Type::SQL_DOUBLE:
			rc = sqlite3_bind_double(m_stmt, idx, std::stod(value.data()));
			break;
		case sqlw::Type::SQL_BLOB:
			rc = sqlite3_bind_blob(
				m_stmt, idx, value.data(), value.size(), SQLITE_TRANSIENT
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
		case sqlw::Type::SQL_INT: {
			std::ostringstream ss;

			ss << sqlite3_column_int(m_stmt, column_idx);

			return ss.str();
		}
		case sqlw::Type::SQL_DOUBLE: {
			std::ostringstream ss;

			ss << sqlite3_column_double(m_stmt, column_idx);

			return ss.str();
	   }
		case sqlw::Type::SQL_TEXT: {
			const std::string::size_type size = sqlite3_column_bytes(m_stmt, column_idx);
			return {
				reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, column_idx)),
				size
			};
		}
		case sqlw::Type::SQL_BLOB: {
			const std::string::size_type size = sqlite3_column_bytes(m_stmt, column_idx);
			return {
				static_cast<const char*>(sqlite3_column_blob(m_stmt, column_idx)),
				size
			};
		}
		case sqlw::Type::SQL_NULL:
			return "";
		default:
			return "";
	}
}
