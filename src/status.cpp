#include "sqlw/status.hpp"
#include "sqlw/statement.hpp"
#include <sstream>
#include <string_view>

std::string sqlw::status::verbose(const sqlw::status::Code code)
{
	std::stringstream ss;

	ss << '(' << static_cast<int>(code) << ") "
	   << sqlite3_errstr(static_cast<int>(code));

	return ss.str();
}

bool sqlw::status::is_ok(const status::Code code)
{
	using C = sqlw::status::Code;

	return code == C::SQLW_OK || code == C::SQLW_DONE || code == C::SQLW_ROW;
}
