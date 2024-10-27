#ifndef SQLW_TRANSACTION_H_
#define SQLW_TRANSACTION_H_

#include "sqlw/connection.hpp"
#include "sqlw/statement.hpp"
#include <concepts>
#include <iostream>
#include <system_error>
#include <type_traits>
#include <utility>

namespace sqlw
{
class Transaction
{
  public:
    struct InvocationArgs
    {
        const std::string_view sql;
        Statement::callback_type callback = nullptr;
    };

    Transaction(sqlw::Connection& con) : m_con(con)
    {
    }

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

    Transaction(Transaction&&) noexcept;
    Transaction& operator=(Transaction&&) noexcept;

    /**
     * Executes all prepared statements. Invokes the `callback`
     * on each execution.
     */
    template <typename... ThingsToBind>
    auto operator()(
        InvocationArgs args,
        const ThingsToBind&&... bindables
    ) noexcept -> std::error_code
    {
        sqlw::Statement stmt{&m_con};
        stmt("SAVEPOINT _savepoint_");

        std::error_code ec = stmt.status();

        if (sqlw::status::Condition::OK != ec)
        {
            return ec;
        }

        ec = stmt.prepare(args.sql).status();

        if (sqlw::status::Condition::OK != ec)
        {
            stmt("ROLLBACK TO _savepoint_");
            return ec;
        }

        size_t i = 0;
        auto bind = [&i, &stmt](const auto& x) {
            i++;

            if constexpr (
                std::is_integral_v<std::remove_cvref_t<decltype(x)>> ||
                std::is_floating_point_v<std::remove_cvref_t<decltype(x)>>)
            {
                stmt.bind(i, x);
            }
            else
            {
                stmt.bind(i, x.first, x.second);
            }

            return sqlw::status::Condition::OK == stmt.status();
        };

        (bind(bindables) && ...);

        ec = stmt.status();

        if (sqlw::status::Condition::OK != ec)
        {
            stmt("ROLLBACK TO _savepoint_");
            return ec;
        }

        stmt(args.callback);
        ec = stmt.status();

        if (sqlw::status::Condition::OK != ec)
        {
            stmt("ROLLBACK TO _savepoint_");
            return ec;
        }

        stmt("RELEASE _savepoint_");

        return stmt.status();
    }

  private:
    sqlw::Connection& m_con;
};
} // namespace sqlw

#endif // SQLW_TRANSACTION_H_
