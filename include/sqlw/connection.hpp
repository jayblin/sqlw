#ifndef SQLW_CONNECTION_H_
#define SQLW_CONNECTION_H_

#include "sqlw/forward.hpp"
#include "sqlite3.h"
#include <filesystem>
#include <gsl/pointers>
#include <string_view>
#include <iostream>

namespace sqlw
{
	class Connection
	{
	public:
		Connection() {};
		Connection(std::string_view filename);

		~Connection();

		Connection(const Connection&) = delete;
		Connection& operator=(const Connection&) = delete;

		Connection(Connection&&) noexcept;
		Connection& operator=(Connection&&) noexcept;

		auto connect(std::string_view file_name) -> void;
		auto handle() const -> sqlite3* { return m_handle; };
		auto status() const -> status::Code { return m_status; }

	private:
		gsl::owner<sqlite3*> m_handle;
		status::Code m_status = status::Code::OK;
	};
}

#endif // SQLW_CONNECTION_H_
