#include "sqlw/connection.hpp"
#include <iostream>

sqlw::Connection::Connection(std::string_view file_name)
{
	connect(file_name);
}

sqlw::Connection::~Connection()
{
	if (nullptr != m_handle)
	{
		sqlite3_close(m_handle);
	}
}

sqlw::Connection::Connection(sqlw::Connection&& other) noexcept
{
	*this = std::move(other);
}

sqlw::Connection& sqlw::Connection::operator=(sqlw::Connection&& other) noexcept
{
	if (this != &other)
	{
		m_handle = other.m_handle;
		m_status = other.m_status;

		other.m_handle = nullptr;
		other.m_status = sqlw::status::Code::_CLOSED_;
	}

	return *this;
}

void sqlw::Connection::connect(std::string_view file_name)
{
	auto rc = sqlite3_open(file_name.data(), &m_handle);

	m_status = static_cast<status::Code>(rc);

	if (m_status != sqlw::status::Code::OK)
	{
		sqlite3_close(m_handle);
	}
}
