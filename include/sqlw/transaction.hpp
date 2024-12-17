#ifndef SQLW_TRANSACTION_H_
#define SQLW_TRANSACTION_H_

#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/statement.hpp"
#include <span>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

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
     * Prepares and executes all statements passed in `sql`.
     * Executes callback on each row fetch.
     */
    auto operator()(
        std::string_view sql,
        Statement::callback_t = nullptr) noexcept
        -> std::error_code;

    /**
     * Prepares and executes all statements passed in `sql`.
     * Binds parameters passed in the span.
     */
    auto operator()(
        std::string_view sql,
        std::span<const Statement::bindable_t>
    ) noexcept -> std::error_code;

    /**
     * Prepares and executes all statements passed in `sql`.
     * Executes callback on each row fetch.
     * Binds parameters passed in the span.
     */
    auto operator()(
        std::string_view sql,
        Statement::callback_t,
        std::span<const Statement::bindable_t>
    ) noexcept -> std::error_code;

    /**
     * Prepares and executes all statements passed in `sql`.
     * Binds parameters passed in the tuple.
     *
     * @note This function can bind parameters only for the first
     * prepared statement.
     */
    template <typename... ThingsToBind>
    requires sqlw::statement::internal::are_bindable<ThingsToBind...>
    auto operator()(
        std::string_view sql,
        std::tuple<ThingsToBind...>&&) noexcept
        -> std::error_code;

    /**
     * Prepares and executes all statements passed in `sql`.
     * Executes callback on each row fetch.
     * Binds parameters passed in the tuple.
     *
     * @note This function can bind parameters only for the first
     * prepared statement.
     */
    template <typename... ThingsToBind>
    requires sqlw::statement::internal::are_bindable<ThingsToBind...>
    auto operator()(
        std::string_view sql,
        Statement::callback_t,
        std::tuple<ThingsToBind...>&&) noexcept
        -> std::error_code;

  private:
    sqlw::Connection* m_con{nullptr};
};

template <typename... ThingsToBind>
    requires sqlw::statement::internal::are_bindable<ThingsToBind...>
std::error_code Transaction::operator()(
    std::string_view sql,
    Statement::callback_t callback,
    std::tuple<ThingsToBind...>&& bindables) noexcept
{
    sqlw::Statement stmt{m_con};

    if (stmt("SAVEPOINT _savepoint_") != sqlw::status::Condition::OK)
    {
        return sqlw::status::Code::SAVEPOINT_ERROR;
    }

    const auto ec = stmt(sql, callback, std::move(bindables));

    if (ec != sqlw::status::Condition::OK)
    {
        if (stmt("ROLLBACK TO _savepoint_") != sqlw::status::Condition::OK) {
            return sqlw::status::Code::ROLLBACK_ERROR;
        }
    }
    else {
        if (stmt("RELEASE _savepoint_") != sqlw::status::Condition::OK)
        {
            return sqlw::status::Code::RELEASE_ERROR;
        }
    }

    return ec;
}

template <typename... ThingsToBind>
requires sqlw::statement::internal::are_bindable<ThingsToBind...>
std::error_code Transaction::operator()(
    std::string_view sql,
    std::tuple<ThingsToBind...>&& bindables) noexcept
{
    return operator()(sql, nullptr, std::move(bindables));
}

/* template <typename... ThingsToBind> */
/* requires sqlw::statement::internal::are_bindable<ThingsToBind...> */
/* std::error_code Transaction::operator()( */
/*     std::string_view sql, */
/*     Statement::callback_t callback) noexcept */
/* { */
/*     return operator()(sql, callback, std::tuple{}); */
/* } */

} // namespace sqlw

#endif // SQLW_TRANSACTION_H_
