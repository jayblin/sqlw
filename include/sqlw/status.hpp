#ifndef SQLW_STATUS_H_
#define SQLW_STATUS_H_

#include "sqlw/forward.hpp"
#include "sqlw/statement.hpp"
#include <string_view>

namespace sqlw::status
{
	constexpr auto view(const Code code) -> std::string_view
	{
		switch (code)
		{
			case status::Code::SQLW_OK:
				return "OK";
			case status::Code::SQLW_ERROR:
				return "ERROR";
			case status::Code::SQLW_INTERNAL:
				return "INTERNAL";
			case status::Code::SQLW_PERM:
				return "PERM";
			case status::Code::SQLW_ABORT:
				return "ABORT";
			case status::Code::SQLW_BUSY:
				return "BUSY";
			case status::Code::SQLW_LOCKED:
				return "LOCKED";
			case status::Code::SQLW_NOMEM:
				return "NOMEM";
			case status::Code::SQLW_READONLY:
				return "READONLY";
			case status::Code::SQLW_INTERRUPT:
				return "INTERRUPT";
			case status::Code::SQLW_IOERR:
				return "IOERR|ROW|DONE";
			case status::Code::SQLW_CORRUPT:
				return "CORRUPT";
			case status::Code::SQLW_NOTFOUND:
				return "NOTFOUND";
			case status::Code::SQLW_FULL:
				return "FULL";
			case status::Code::SQLW_CANTOPEN:
				return "CANTOPEN";
			case status::Code::SQLW_PROTOCOL:
				return "PROTOCOL";
			case status::Code::SQLW_EMPTY:
				return "EMPTY";
			case status::Code::SQLW_SCHEMA:
				return "SCHEMA";
			case status::Code::SQLW_TOOBIG:
				return "TOOBIG";
			case status::Code::SQLW_CONSTRAINT:
				return "CONSTRAINT";
			case status::Code::SQLW_MISMATCH:
				return "MISMATCH";
			case status::Code::SQLW_MISUSE:
				return "MISUSE";
			case status::Code::SQLW_NOLFS:
				return "NOLFS";
			case status::Code::SQLW_AUTH:
				return "AUTH";
			case status::Code::SQLW_FORMAT:
				return "FORMAT";
			case status::Code::SQLW_RANGE:
				return "RANGE";
			case status::Code::SQLW_NOTADB:
				return "NOTADB";
			case status::Code::SQLW_NOTICE:
				return "NOTICE";
			case status::Code::SQLW_WARNING:
				return "WARNING";
			default:
				return "UNKNOWN";
		}
	}

	auto verbose(const Code) -> std::string;

	auto is_ok(const Code) -> bool;
} // namespace sqlw::status

#endif // SQLW_STATUS_H_
