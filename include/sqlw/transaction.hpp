#ifndef SQLW_TRANSACTION_H_
#define SQLW_TRANSACTION_H_

#include "sqlw/connection.hpp"
#include "sqlw/statement.hpp"
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include "gsl/util"

namespace sqlw
{
class Transaction
{
  public:
    Transaction(sqlw::Connection* con) : m_con(con)
    {
    }

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

    Transaction(Transaction&& other) noexcept
    {
        *this = std::move(other);
    }

    Transaction& operator=(Transaction&& other) noexcept
    {
        m_con = other.m_con;
        other.m_con = nullptr;

        return *this;
    }

    /**
     * Executes all prepared statements provided in `sql` in a transaction.
     * Invokes the `callback` on each execution.
     */
    template <typename... ThingsToBind>
    auto operator()(
        std::string_view sql,
        Statement::callback_type callback,
        const std::tuple<ThingsToBind...>& bindables) noexcept
        -> std::error_code;

    /**
     * Executes all prepared statements provided in `sql` in a transaction.
     */
    template <typename... ThingsToBind>
    auto operator()(
        std::string_view sql,
        const std::tuple<ThingsToBind...>& bindables) noexcept
        -> std::error_code;

    /**
     * Executes all prepared statements provided in `sql` in a transaction.
     */
    template <typename... ThingsToBind>
    auto operator()(
        std::string_view sql,
        Statement::callback_type callback = nullptr) noexcept
        -> std::error_code;

  private:
    sqlw::Connection* m_con{nullptr};

    template <typename T>
    auto bind(sqlw::Statement& stmt, const T& x, gsl::index& i) -> bool
    {
        i++;

        if constexpr (
            std::is_integral_v<std::remove_cvref_t<T>> ||
            std::is_floating_point_v<std::remove_cvref_t<T>>)
        {
            stmt.bind(i, x);
        }
        else
        {
            stmt.bind(i, x.first, x.second);
        }

        return sqlw::status::Condition::OK == stmt.status();
    }
};

template <typename... ThingsToBind>
std::error_code Transaction::operator()(
    std::string_view sql,
    Statement::callback_type callback,
    const std::tuple<ThingsToBind...>& bindables) noexcept
{
    sqlw::Statement stmt{m_con};
    stmt("SAVEPOINT _savepoint_");

    std::error_code ec = stmt.status();

    if (sqlw::status::Condition::OK != ec)
    {
        return ec;
    }

    ec = stmt.prepare(sql).status();

    if (sqlw::status::Condition::OK != ec)
    {
        stmt("ROLLBACK TO _savepoint_");
        return ec;
    }

    if constexpr (
        std::tuple_size<std::remove_cvref_t<decltype(bindables)>>() > 0)
    {
        gsl::index i = 0;
        std::apply(
            [&](auto&&... bindable) { (bind(stmt, bindable, i) && ...); },
            bindables);

        ec = stmt.status();
    }

    if (sqlw::status::Condition::OK != ec)
    {
        stmt("ROLLBACK TO _savepoint_");
        return ec;
    }

    stmt(callback);
    ec = stmt.status();

    if (sqlw::status::Condition::OK != ec)
    {
        stmt("ROLLBACK TO _savepoint_");
        return ec;
    }

    stmt("RELEASE _savepoint_");

    return stmt.status();
}

template <typename... ThingsToBind>
std::error_code Transaction::operator()(
    std::string_view sql,
    const std::tuple<ThingsToBind...>& bindables) noexcept
{
    return operator()(sql, nullptr, bindables);
}

template <typename... ThingsToBind>
std::error_code Transaction::operator()(
    std::string_view sql,
    Statement::callback_type callback) noexcept
{
    return operator()(sql, callback, std::tuple{});
}

} // namespace sqlw

#endif // SQLW_TRANSACTION_H_
