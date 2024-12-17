#include "sqlw/statement.hpp"
#include "sqlw/cmake_vars.h"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/utils.hpp"
#include <charconv>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>

sqlw::Statement::Statement(sqlw::Connection* con) : m_connection(con)
{
}

sqlw::Statement::~Statement()
{
    sqlite3_finalize(m_stmt);
}

sqlw::Statement::Statement(sqlw::Statement&& other) noexcept
{
    *this = std::move(other);
}

sqlw::Statement& sqlw::Statement::operator=(sqlw::Statement&& other) noexcept
{
    if (this != &other)
    {
        m_status = other.m_status;
        m_stmt = other.m_stmt;
        m_connection = other.m_connection;
        m_unused_sql = other.m_unused_sql;

        other.m_stmt = nullptr;
        other.m_connection = nullptr;
        other.m_unused_sql = nullptr;
        other.m_status = status::Code::CLOSED_HANDLE;
    }

    return *this;
}

sqlw::Statement& sqlw::Statement::exec(
    sqlw::Statement::callback_t callback) noexcept
{
    auto rc = sqlite3_step(m_stmt);
    m_status = status::Code{rc};

    if (sqlw::status::Condition::OK != m_status)
    {
        return *this;
    }

    const auto col_count = sqlite3_data_count(m_stmt);

    if (0 == col_count)
    {
        m_status = status::Code{SQLITE_DONE};
    }

    if (status::Condition::DONE == m_status)
    {
        return *this;
    }

    for (auto i = 0; i < col_count; i++)
    {
        const auto t = static_cast<sqlw::Type>(sqlite3_column_type(m_stmt, i));

        if (callback)
        {
            callback(
                {col_count,
                 sqlite3_column_name(m_stmt, i),
                 t,
                 column_value(t, i)});
        }
    }

    return *this;
}

sqlw::Statement& sqlw::Statement::prepare(std::string_view sql) noexcept
{
    auto rc = sqlite3_prepare_v2(
        m_connection->handle(), sql.data(), sql.size(), &m_stmt, &m_unused_sql);

    m_status = status::Code{rc};

    return *this;
}

std::error_code sqlw::Statement::operator()(
    sqlw::Statement::callback_t callback,
    sqlw::Statement::unused_params_t unused_params) noexcept
{
    size_t iter = 0;
    do
    {
        if (sqlw::status::Condition::OK != m_status)
        {
            break;
        }

        this->exec(callback);
        iter++;

        std::string_view unused{m_unused_sql};

        if (sqlw::status::Condition::DONE == m_status && !unused.empty())
        {
            sqlite3_finalize(m_stmt);
            this->prepare(unused);

            if (sqlw::status::Condition::OK != m_status)
            {
                break;
            }

            unused_params = this->bind(unused_params);
        }
        else if (sqlw::status::Condition::ROW != m_status)
        {
            break;
        }
    } while (iter < SQLW_EXEC_LIMIT);
    m_unused_sql = nullptr;

    return m_status;
}

std::error_code sqlw::Statement::operator()(
    std::string_view sql,
    sqlw::Statement::callback_t callback) noexcept
{
    this->prepare(sql);

    if (sqlw::status::Condition::OK == m_status)
    {
        return operator()(callback);
    }

    return m_status;
}

std::error_code sqlw::Statement::operator()(
    std::string_view sql,
    std::span<const sqlw::Statement::bindable_t> params)
{
    return operator()(sql, nullptr, params);
}

sqlw::Statement::unused_params_t sqlw::Statement::bind(
    std::span<const sqlw::Statement::bindable_t> params) noexcept
{
    if (params.size() == 0)
    {
        return params;
    }

    size_t expected_param_count = sqlite3_bind_parameter_count(m_stmt);

    size_t i = 0;
    for (; i < params.size() && i < expected_param_count; i++)
    {
        this->bind(i + 1, params[i].first, params[i].second);

        if (sqlw::status::Condition::OK != m_status)
        {
            break;
        }
    }

    return params.last(params.size() - i);
}

std::error_code sqlw::Statement::operator()(
    std::string_view sql,
    sqlw::Statement::callback_t callback,
    std::span<const sqlw::Statement::bindable_t> params)
{
    prepare(sql);

    if (sqlw::status::Condition::OK != m_status)
    {
        return m_status;
    }

    auto unused_params = this->bind(params);

    if (sqlw::status::Condition::OK != m_status)
    {
        return m_status;
    }

    return operator()(callback, unused_params);
}

// @todo Определять динамически, когда использовать SQLITE_TRANSIENT,
// а когда SQLITE_STATIC.
sqlw::Statement& sqlw::Statement::bind(
    int idx,
    std::string_view value,
    sqlw::Type t) noexcept
{
    int rc = 0;

    switch (t)
    {
    case sqlw::Type::SQL_TEXT:
        rc = sqlite3_bind_text(
            m_stmt, idx, value.data(), value.size(), SQLITE_TRANSIENT);
        break;
    case sqlw::Type::SQL_DOUBLE: {
        // can't use from_chars on mac((((
        // https://en.cppreference.com/w/cpp/compiler_support/17#C.2B.2B17_library_features
        double v;
        std::errc ec = sqlw::utils::to_double(value, v);

        if (ec != std::errc())
        {
            m_status = status::Code{SQLITE_MISUSE};
            return *this;
        }

        rc = sqlite3_bind_double(m_stmt, idx, v);
        break;
    }
    case sqlw::Type::SQL_BLOB:
        rc = sqlite3_bind_blob(
            m_stmt, idx, value.data(), value.size(), SQLITE_TRANSIENT);
        break;
    case sqlw::Type::SQL_INT: {
        int v;
        std::from_chars_result fcr =
            std::from_chars(value.data(), value.data() + value.size(), v);

        if (fcr.ec != std::errc())
        {
            m_status = status::Code{SQLITE_MISUSE};
            return *this;
        }

        rc = sqlite3_bind_int(m_stmt, idx, v);
        break;
    }
    case sqlw::Type::SQL_NULL:
        rc = sqlite3_bind_null(m_stmt, idx);
        break;
    }

    m_status = status::Code{rc};

    return *this;
}

sqlw::Statement& sqlw::Statement::bind(int idx, double value) noexcept
{
    int rc = sqlite3_bind_double(m_stmt, idx, value);

    m_status = status::Code{rc};

    return *this;
}

sqlw::Statement& sqlw::Statement::bind(int idx, int value) noexcept
{
    int rc = sqlite3_bind_int(m_stmt, idx, value);

    m_status = status::Code{rc};

    return *this;
}

std::string sqlw::Statement::column_value(sqlw::Type type, int column_idx)
{
    switch (type)
    {
    case sqlw::Type::SQL_INT: {
        std::ostringstream ss;

        ss << sqlite3_column_int(m_stmt, column_idx);

        return ss.str();
    }
    case sqlw::Type::SQL_DOUBLE: {
        std::ostringstream ss;

        ss << sqlite3_column_double(m_stmt, column_idx);

        return ss.str();
    }
    case sqlw::Type::SQL_TEXT: {
        const std::string::size_type size =
            sqlite3_column_bytes(m_stmt, column_idx);
        return {
            reinterpret_cast<const char*>(
                sqlite3_column_text(m_stmt, column_idx)),
            size};
    }
    case sqlw::Type::SQL_BLOB: {
        const std::string::size_type size =
            sqlite3_column_bytes(m_stmt, column_idx);
        return {
            static_cast<const char*>(sqlite3_column_blob(m_stmt, column_idx)),
            size};
    }
    case sqlw::Type::SQL_NULL:
        return "";
    default:
        return "";
    }
}
