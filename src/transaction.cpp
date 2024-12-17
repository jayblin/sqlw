#include "sqlw/transaction.hpp"

std::error_code sqlw::Transaction::operator()(
    std::string_view sql,
    sqlw::Statement::callback_t callback,
    std::span<const sqlw::Statement::bindable_t> params) noexcept
{
    sqlw::Statement stmt{m_con};

    if (stmt("SAVEPOINT _savepoint_") != sqlw::status::Condition::OK)
    {
        return sqlw::status::Code::SAVEPOINT_ERROR;
    }

    const auto ec = stmt(sql, callback, params);

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

std::error_code sqlw::Transaction::operator()(
    std::string_view sql,
    std::span<const sqlw::Statement::bindable_t> params) noexcept
{
    return operator()(sql, nullptr, params);
}

std::error_code sqlw::Transaction::operator()(
    std::string_view sql,
    sqlw::Statement::callback_t callback) noexcept
{
    return operator()(sql, callback, {});
}
