#ifndef SQLW_STATEMENT_H_
#define SQLW_STATEMENT_H_

#include "gsl/pointers"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include <concepts>
#include <functional>
#include <gsl/util>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>

namespace sqlw
{
class Statement;

namespace statement::internal
{
struct ExecArgs
{
    int column_count;
    std::string_view column_name;
    sqlw::Type column_type;
    std::string_view column_value;
};

typedef std::function<void(ExecArgs)> callback_t;
typedef std::pair<std::string_view, sqlw::Type> bindable_t;

template <typename... Ts>
concept are_bindable =
    ((std::same_as<bindable_t, Ts> || std::is_integral_v<Ts> ||
      std::is_floating_point_v<Ts>) &&
     ...);
} // namespace statement::internal

class Statement
{
  public:
    typedef statement::internal::ExecArgs ExecArgs;
    typedef statement::internal::callback_t callback_t;
    typedef statement::internal::bindable_t bindable_t;
    typedef std::span<const bindable_t> unused_params_t;

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

    auto prepare(std::string_view sql) noexcept -> Statement&;

    auto bind(int idx, std::string_view value, Type t = Type::SQL_TEXT) noexcept
        -> Statement&;

    auto bind(int idx, double value) noexcept -> Statement&;

    auto bind(int idx, int value) noexcept -> Statement&;

    auto bind(std::span<const bindable_t>) noexcept -> unused_params_t;

    /**
     * Executes the first statement up until ";".
     * Usefull for INSERT/UPDATE/DELETE.
     */
    auto exec(callback_t = nullptr) noexcept -> Statement&;

    auto status() const noexcept -> std::error_code
    {
        return m_status;
    }

    /**
     * Prepares and executes all statements passed in `sql`.
     * Executes callback on each row fetch.
     */
    auto operator()(std::string_view sql, callback_t = nullptr) noexcept
        -> std::error_code;

    /**
     * Prepares and executes all statements passed in `sql`.
     * Binds parameters passed in the span.
     */
    auto operator()(std::string_view sql, std::span<const bindable_t>)
        -> std::error_code;

    /**
     * Prepares and executes all statements passed in `sql`.
     * Executes callback on each row fetch.
     * Binds parameters passed in the span.
     */
    auto operator()(
        std::string_view sql,
        callback_t,
        std::span<const bindable_t>) -> std::error_code;

    /**
     * Prepares and executes all statements passed in `sql`.
     * Binds parameters passed in the tuple.
     *
     * @note This function can bind parameters only for the first
     * prepared statement.
     */
    template <typename... ThingsToBind>
        requires statement::internal::are_bindable<ThingsToBind...>
    auto operator()(std::string_view sql, std::tuple<ThingsToBind...>&& params)
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
        requires statement::internal::are_bindable<ThingsToBind...>
    auto operator()(
        std::string_view sql,
        callback_t callback,
        std::tuple<ThingsToBind...>&& params) -> std::error_code;

    auto operator()(callback_t = nullptr, unused_params_t = {}) noexcept
        -> std::error_code;

  private:
    Connection* m_connection{nullptr};
    gsl::owner<sqlite3_stmt*> m_stmt{nullptr};
    std::error_code m_status{status::Code{}};
    gsl::owner<const char*> m_unused_sql{nullptr};
    std::string_view m_sql_string{};

    template <typename T>
    auto internal_bind(sqlw::Statement& stmt, const T& x, size_t index) -> bool;
};

template <typename T>
bool Statement::internal_bind(sqlw::Statement& stmt, const T& x, size_t i)
{
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

template <typename... ThingsToBind>
    requires statement::internal::are_bindable<ThingsToBind...>
auto Statement::operator()(
    std::string_view sql,
    callback_t callback,
    std::tuple<ThingsToBind...>&& params) -> std::error_code
{
    this->prepare(sql);

    if (sqlw::status::Condition::OK != m_status)
    {
        return m_status;
    }

    if constexpr (std::tuple_size<std::remove_cvref_t<decltype(params)>>() > 0)
    {
        size_t expected_param_count = sqlite3_bind_parameter_count(m_stmt);
        size_t i = 1;
        std::apply(
            [&](auto&&... param) {
                ((i <= expected_param_count &&
                  this->internal_bind(*this, param, i) && (++i)) &&
                 ...);
            },
            params);

        if (i <= std::tuple_size<std::remove_cvref_t<decltype(params)>>())
        {
            m_status = sqlw::status::Code::UNUSED_PARAMETERS_ERROR;
        }

        if (sqlw::status::Condition::OK != m_status)
        {
            return m_status;
        }
    }

    if (sqlw::status::Condition::OK != m_status)
    {
        return m_status;
    }

    return operator()(callback);
}

template <typename... ThingsToBind>
    requires statement::internal::are_bindable<ThingsToBind...>
auto Statement::operator()(
    std::string_view sql,
    std::tuple<ThingsToBind...>&& params) -> std::error_code
{
    return operator()(sql, nullptr, std::move(params));
}

} // namespace sqlw

#endif // SQLW_STATEMENT_H_
