#include "sqlw/forward.hpp"
#include <system_error>

static bool is_code_ok(int rc)
{
    return rc == SQLITE_OK || rc == SQLITE_DONE || rc == SQLITE_ROW;
}

namespace sqlw::status
{
struct ErrorCategory : std::error_category
{
    const char* name() const noexcept override final
    {
        return "sqlw error";
    }

    std::string message(int ec) const override final
    {
        switch (sqlw::status::Code{ec})
        {
        case Code::CLOSED_HANDLE:
            return "object's handle is closed";
        case Code::ROLLBACK_ERROR:
            return "couldn't ROLLBACK";
        case Code::SAVEPOINT_ERROR:
            return "error at SAVEPOINT creation";
        case Code::RELEASE_ERROR:
            return "error on savepoint RELEASE";
        case Code::UNUSED_PARAMETERS_ERROR:
            return "some paramaters where not bound";
            break;
        }

        return sqlite3_errstr(ec);
    }
};

struct ConditionCategory : std::error_category
{
    const char* name() const noexcept override final
    {
        return "sqlw condition";
    }

    std::string message(int ec) const override final
    {
        using C = sqlw::status::Condition;
        switch (static_cast<C>(ec))
        {
        case C::OK:
            return "no errors";
        case C::ROW:
            return "another row available";
        case C::DONE:
            return "no more rows available";
        case C::ERROR:
            return "error while performing an operation";
        case C::CLOSED_HANDLE:
            return "object's handle is closed";
        }

        return "(unknown condition)";
    }

    bool equivalent(const std::error_code& ec, int cond)
        const noexcept override final
    {
        const std::error_category& sqlw_error_category =
            std::error_code{Code{}}.category();

        if (ec.category() != sqlw_error_category)
        {
            return false;
        }

        switch (static_cast<Condition>(cond))
        {
        case Condition::OK:
            return is_code_ok(ec.value());
        case Condition::ERROR:
            return !is_code_ok(ec.value());
            return false;
        case Condition::DONE:
            return ec.value() == SQLITE_DONE;
        case Condition::ROW:
            return ec.value() == SQLITE_ROW;
        case Condition::CLOSED_HANDLE:
            return ec == Code::CLOSED_HANDLE;
        default:
            return false;
        }
    }
};

const ErrorCategory error_category{};
const ConditionCategory condition_category{};
} // namespace sqlw::status

std::error_code sqlw::status::make_error_code(sqlw::status::Code ec)
{
    return {static_cast<int>(ec), sqlw::status::error_category};
}

std::error_condition sqlw::status::make_error_condition(
    sqlw::status::Condition ec)
{
    return {static_cast<int>(ec), sqlw::status::condition_category};
}
