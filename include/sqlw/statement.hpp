#ifndef SQLW_STATEMENT_H_
#define SQLW_STATEMENT_H_

#include "gsl/pointers"
#include "sqlw/cmake_vars.h"
#include "sqlw/concepts.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>

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

		using callback_type = std::function<void(ExecArgs)>;

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
		auto bind(int idx, std::string_view value, Type t = Type::SQL_TEXT)
		    noexcept -> Statement&;

		auto bind(int idx, double value) noexcept -> Statement&;

		auto bind(int idx, int value) noexcept -> Statement&;

		auto prepare(std::string_view sql) -> Statement&;

		/**
		 * Executes the first statement up until ";".
		 * Usefull for INSERT/UPDATE/DELETE.
		 */
		auto exec(callback_type callback = nullptr) -> Statement&;

		/**
		 * Operator() executes statement and returns an object of `T`, that
		 * must be able to handle data that is received from database. Useful
		 * for SELECT.
		 */
		template<class T>
		requires can_be_used_by_statement<T>
		auto operator()(callback_type callback = nullptr) -> T;

		template<class T>
		requires can_be_used_by_statement<T>
		auto operator()(std::string_view sql, callback_type callback = nullptr)
		    -> T;

        /**
         * Executes all prepared statements. Invokes the `callback`
         * on each execution.
         */
		auto operator()(callback_type callback = nullptr) -> void;

        /**
         * Prepares statements provided in `sql`. Executes them.
         * Invokes the `callback` on each execution.
         */
		auto operator()(std::string_view sql, callback_type callback = nullptr)
		    -> void;

        auto status() const -> std::error_code
        {
            return m_status;
        }

	private:
		Connection* m_connection {nullptr};
		gsl::owner<sqlite3_stmt*> m_stmt {nullptr};
        std::error_code m_status {status::Code{}};
		gsl::owner<const char*> m_unused_sql {nullptr};
	};

	template<class T>
	requires can_be_used_by_statement<T>
	T Statement::operator()(Statement::callback_type callback)
	{
		T obj;
		size_t iter = 0;

		do
		{
			exec(callback);
			iter++;

			const auto col_count = sqlite3_data_count(m_stmt);

			if (status::Condition::OK == m_status || 0 == col_count)
			{
				break;
			}

			obj.row(col_count);

			for (auto i = 0; i < col_count; i++)
			{
				const auto t = static_cast<sqlw::Type>(
				    sqlite3_column_type(m_stmt, i)
				);

				obj.column(sqlite3_column_name(m_stmt, i), t, column_value(t, i));
			}
		}
		while (status::Condition::ROW == m_status && iter < SQLW_EXEC_LIMIT);

		return obj;
	}

	template<class T>
	requires can_be_used_by_statement<T>
	T Statement::operator()(
	    std::string_view sql,
	    Statement::callback_type callback
	)
	{
		prepare(sql);

		return operator()<T>(callback);
	}
} // namespace sqlw

#endif // SQLW_STATEMENT_H_
