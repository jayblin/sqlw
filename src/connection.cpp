#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"

sqlw::Connection::Connection(std::string_view file_name)
{
	connect(file_name);
}

sqlw::Connection::~Connection()
{
	this->close();
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
		other.m_status = sqlw::status::Code::CLOSED_HANDLE;
	}

	return *this;
}

void sqlw::Connection::connect(std::string_view file_name)
{
	auto rc = sqlite3_open(file_name.data(), &m_handle);

	m_status = static_cast<status::Code>(rc);

	if (status::Condition::OK != m_status)
	{
		this->close();
	}
}

void sqlw::Connection::close()
{
	if (nullptr != m_handle)
	{
		sqlite3_close(m_handle);
		m_handle = nullptr;
	}
}
