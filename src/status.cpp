#include "sqlw/status.hpp"
#include "sqlw/sqlite3.hpp"
#include "sqlw/statement.hpp"
#include <sstream>
#include <string_view>

std::string sqlw::status::verbose(const sqlw::Sqlite3& sqlite)
{
	return sqlite3_errmsg(sqlite.m_db);
}

std::string sqlw::status::verbose(const sqlw::Statement& stmt)
{
	return verbose(stmt.status());
}

static std::string _verbose(const sqlw::status::Code code)
{
	switch (code)
	{
		case sqlw::status::Code::OK:
			return "Successful result";
		case sqlw::status::Code::ERROR:
			return "Generic error";
		case sqlw::status::Code::INTERNAL:
			return "Internal logic error in SQLite";
		case sqlw::status::Code::PERM:
			return "Access permission denied";
		case sqlw::status::Code::ABORT:
			return "Callback routine requested an abort";
		case sqlw::status::Code::BUSY:
			return "The database file is locked";
		case sqlw::status::Code::LOCKED:
			return "A table in the database is locked";
		case sqlw::status::Code::NOMEM:
			return "A malloc() failed";
		case sqlw::status::Code::READONLY:
			return "Attempt to write a readonly database";
		case sqlw::status::Code::INTERRUPT:
			return "Operation terminated by sqlite3_interrupt(";
		case sqlw::status::Code::IOERR:
			return "Some kind of disk I/O error occurred";
		case sqlw::status::Code::CORRUPT:
			return "The database disk image is malformed";
		case sqlw::status::Code::NOTFOUND:
			return "Unknown opcode in sqlite3_file_control()";
		case sqlw::status::Code::FULL:
			return "Insertion failed because database is full";
		case sqlw::status::Code::CANTOPEN:
			return "Unable to open the database file";
		case sqlw::status::Code::PROTOCOL:
			return "Database lock protocol error";
		case sqlw::status::Code::EMPTY:
			return "Internal use only";
		case sqlw::status::Code::SCHEMA:
			return "The database schema changed";
		case sqlw::status::Code::TOOBIG:
			return "String or BLOB exceeds size limit";
		case sqlw::status::Code::CONSTRAINT:
			return "Abort due to constraint violation";
		case sqlw::status::Code::MISMATCH:
			return "Data type mismatch";
		case sqlw::status::Code::MISUSE:
			return "Library used incorrectly";
		case sqlw::status::Code::NOLFS:
			return "Uses OS features not supported on host";
		case sqlw::status::Code::AUTH:
			return "Authorization denied";
		case sqlw::status::Code::FORMAT:
			return "Not used";
		case sqlw::status::Code::RANGE:
			return "2nd parameter to sqlite3_bind out of range";
		case sqlw::status::Code::NOTADB:
			return "File opened that is not a database file";
		case sqlw::status::Code::NOTICE:
			return "Notifications from sqlite3_log()";
		case sqlw::status::Code::WARNING:
			return "Warnings from sqlite3_log()";
		default:
			return "Unknown sqlite error code";
	}
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


bool sqlw::status::is_ok(const Sqlite3& sqlite)
{
	return is_ok(sqlite.status());
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

