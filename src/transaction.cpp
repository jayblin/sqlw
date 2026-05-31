#include "sqlw/transaction.hpp"
#include "sqlw/forward.hpp"

std::error_code sqlw::Transaction::operator()(
    std::string_view sql,
    sqlw::Statement::callback_t callback,
    std::span<const sqlw::Statement::bindable_t> params) noexcept
{

    if (sqlw::Statement{m_con}("SAVEPOINT _savepoint_") !=
        sqlw::status::Condition::OK)
    {
        return sqlw::status::Code::SAVEPOINT_ERROR;
    }

    sqlw::Statement stmt{m_con};

    const auto ec = stmt(sql, callback, params);

    m_target_stmt_meta = stmt.meta();

    if (ec != sqlw::status::Condition::OK)
    {
        if (nullptr != m_con)
        {
            if (ec == sqlw::status::Condition::INVALID_PARAM)
            {
                m_target_stmt_error_message = ec.message();
            }
            else
            {
                m_target_stmt_error_message = sqlite3_errmsg(m_con->handle());
            }
        }

        if (sqlw::Statement{m_con}("ROLLBACK TO _savepoint_") !=
            sqlw::status::Condition::OK)
        {
            return sqlw::status::Code::ROLLBACK_ERROR;
        }
    }

    if (sqlw::Statement{m_con}("RELEASE _savepoint_") !=
        sqlw::status::Condition::OK)
    {
        return sqlw::status::Code::RELEASE_ERROR;
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
