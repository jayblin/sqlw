#include "sqlw/status.hpp"
#include "sqlw/statement.hpp"
#include <sstream>
#include <string_view>

std::string sqlw::status::verbose(const sqlw::Statement& stmt)
{
	return verbose(stmt.status());
}

std::string sqlw::status::verbose(const sqlw::status::Code code)
{
	std::stringstream ss;

	ss << '(' << static_cast<int>(code) << ") "
		<< sqlw::status::view(code) << ": "
		<< _verbose(code)
	;

	return ss.str();
}


bool sqlw::status::is_ok(const Statement& stmt)
{
	return is_ok(stmt.status());
}

bool sqlw::status::is_ok(const status::Code code)
{
	using C = sqlw::status::Code;

	return code == C::OK || code == C::DONE || code == C::ROW;
}

