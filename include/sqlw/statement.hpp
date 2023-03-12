#ifndef SQLW_STATEMENT_H_
#define SQLW_STATEMENT_H_

#include "sqlw/concepts.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/cmake_vars.h"
#include "sqlite3.h"
#include <functional>
#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <iostream>
#include <span>
#include "gsl/pointers"

namespace sqlw
{
	class Statement
	{
	public:
		struct ExecArgs
		{
			int column_count;
			std::string_view column_name;
			sqlw::Type column_type;
			std::string_view column_value;
		};

		Statement(Connection* connection);

		~Statement();

		Statement(const Statement&) = delete;
		Statement& operator=(const Statement&) = delete;

		Statement(Statement&&) noexcept;
		Statement& operator=(Statement&&) noexcept;

		/**
		 * Returns the value of a column.
		 */
		auto column_value(Type type, int column_idx) -> std::string;

		/**
		 * Binds a value to argument at position `idx`.
		 */
		auto bind(int idx, std::string_view value, Type t) -> Statement&;

		/**
		 * Executes the statement once.
		 * Usefull for INSERT/UPDATE/DELETE.
		 */
		auto exec() -> Statement&;

		auto exec(std::function<void (ExecArgs)>) -> Statement&;

		auto prepare(std::string_view sql) -> void;

		/**
		 * Executes statement and returns an object of `T`, that must be able to
		 * handle data that is received from database.
		 * Useful for SELECT.
		 */
		template <class T>
			requires can_be_used_by_statement<T>
		auto operator()(std::string_view sql) -> T;

		auto operator()(std::string_view sql) -> void;

		auto operator()(
			std::string_view sql,
			std::function<void (ExecArgs)> callback
		) -> void;

		constexpr auto status() const -> status::Code { return m_status; }

	private:
		Connection* 				m_connection {nullptr};
		gsl::owner<sqlite3_stmt*> 	m_stmt {nullptr};
		status::Code 				m_status {status::Code::OK};
		gsl::owner<const char*> 	m_unused_sql {nullptr};
	};

	template <class T>
		requires can_be_used_by_statement<T>
	T Statement::operator()(std::string_view sql)
	{
		T obj;
		size_t iter = 0;

		prepare(sql);

		do
		{
			exec();
			iter++;

			const auto col_count = sqlite3_data_count(m_stmt);

			if (status::Code::DONE == m_status || 0 == col_count)
			{
				break;
			}

			obj.row(col_count);

			for (auto i = 0; i < col_count; i++)
			{
				const auto t = static_cast<sqlw::Type>(
					sqlite3_column_type(m_stmt, i)
				);

				obj.column(
					sqlite3_column_name(m_stmt, i),
					t,
					column_value(t, i)
				);
			}
		}
		while (status::Code::ROW == m_status && iter < SQLW_EXEC_LIMIT);

		return obj;
	}
}

#endif // SQLW_STATEMENT_H_
