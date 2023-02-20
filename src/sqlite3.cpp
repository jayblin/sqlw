#include "sqlw/sqlite3.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/statement.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

sqlw::Sqlite3::Sqlite3(std::string_view file_name)
{
	std::cout << "bout to connect to " << file_name << '\n';
	connect(file_name);
}

sqlw::Sqlite3::Sqlite3()
{
}

sqlw::Sqlite3::~Sqlite3()
{
	std::cout << "destructing db \n";

	if (nullptr != m_db)
	{
		std::cout << " bout to close m_db is " << m_db << '\n';
		sqlite3_close(m_db);
	}

	delete m_err_msg;
}

sqlw::Sqlite3::Sqlite3(sqlw::Sqlite3&& other) noexcept
{
	std::cout << "bout to mooove\n";
	*this = std::move(other);
}

sqlw::Sqlite3& sqlw::Sqlite3::operator=(sqlw::Sqlite3&& other) noexcept
{
	std::cout << "mooooovimg\n";
	if (this != &other)
	{
		m_db = other.m_db;
		m_status = other.m_status;
		m_err_msg = other.m_err_msg;

		other.m_db = nullptr;
		// @todo: is this right ???
		delete other.m_err_msg;
	}

	return *this;
}

void sqlw::Sqlite3::connect(std::string_view file_name)
{
	std::cout << "connectiong to " << file_name.data() << "\n";
	auto rc = sqlite3_open(file_name.data(), &m_db);

	m_status = static_cast<status::Code>(rc);

	std::cout << "connected with code " << rc << '\n';

	if (m_status != sqlw::status::Code::OK)
	{
		std::cout << sqlite3_errmsg(m_db) << '\n';
		sqlite3_close(m_db);
	}
}

void sqlw::Sqlite3::exec(
    std::string_view sql,
    sqlite3_callback callback,
    void* obj
)
{
	auto rc = sqlite3_exec(m_db, sql.data(), callback, obj, &m_err_msg);

	m_status = static_cast<status::Code>(rc);
}
