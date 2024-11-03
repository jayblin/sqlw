#include "sqlw/transaction.hpp"

std::error_code sqlw::Transaction::operator()(
    std::string_view sql,
    Statement::callback_type callback,
    std::span<const std::pair<std::string_view, sqlw::Type>> params) noexcept
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

    for (size_t i = 0; i < params.size(); i++)
    {
        stmt.bind(i + 1, params[i].first, params[i].second);
        ec = stmt.status();

        if (sqlw::status::Condition::OK != ec)
        {
            break;
        }
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

std::error_code sqlw::Transaction::operator()(
    std::string_view sql,
    std::span<const std::pair<std::string_view, sqlw::Type>> params) noexcept
{
    return operator()(sql, nullptr, params);
}
