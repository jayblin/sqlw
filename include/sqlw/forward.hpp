#ifndef SQLW_FORWARD_H_
#define SQLW_FORWARD_H_

#include "sqlite3.h"
#include <system_error>
#include <type_traits>

namespace sqlw
{
	enum class Type
	{
		SQL_BLOB = SQLITE_BLOB,
		SQL_DOUBLE = SQLITE_FLOAT,
		SQL_INT = SQLITE_INTEGER,
		SQL_NULL = SQLITE_NULL,
		SQL_TEXT = SQLITE_TEXT,
	};
} // namespace sqlw

namespace sqlw::status
{
    enum class Code : int
    {
        CLOSED_HANDLE = -100,
    };

    enum class Condition : int
    {
        OK = 0,
        ROW,
        DONE,
        ERROR,
        CLOSED_HANDLE,
    };

    std::error_code make_error_code(sqlw::status::Code ec);
    std::error_condition make_error_condition(sqlw::status::Condition ec);

} // namespace status

namespace std
{
    template <>
    struct is_error_code_enum<sqlw::status::Code> : true_type {};

    template <>
    struct is_error_condition_enum<sqlw::status::Condition> : true_type {};
}

#endif // SQLW_FORWARD_H_
