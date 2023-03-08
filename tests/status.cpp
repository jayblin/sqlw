#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/status.hpp"
#include <cstdio>
#include <gtest/gtest.h>
#include <vector>

TEST(Status, can_determine_ok_status)
{
	ASSERT_TRUE(sqlw::status::is_ok(sqlw::status::Code::OK));
	ASSERT_TRUE(sqlw::status::is_ok(sqlw::status::Code::DONE));
	ASSERT_TRUE(sqlw::status::is_ok(sqlw::status::Code::ROW));
}

TEST(Status, can_report_readable_error)
{
	std::cout << "---PRINTING POSSIBLE SQLITE ERROR CODES---\n";

	std::vector<sqlw::status::Code> codes = {
		sqlw::status::Code::OK,
		sqlw::status::Code::ERROR,
		sqlw::status::Code::INTERNAL,
		sqlw::status::Code::PERM,
		sqlw::status::Code::ABORT,
		sqlw::status::Code::BUSY,
		sqlw::status::Code::LOCKED,
		sqlw::status::Code::NOMEM,
		sqlw::status::Code::READONLY,
		sqlw::status::Code::INTERRUPT,
		sqlw::status::Code::IOERR,
		sqlw::status::Code::CORRUPT,
		sqlw::status::Code::NOTFOUND,
		sqlw::status::Code::FULL,
		sqlw::status::Code::CANTOPEN,
		sqlw::status::Code::PROTOCOL,
		sqlw::status::Code::EMPTY,
		sqlw::status::Code::SCHEMA,
		sqlw::status::Code::TOOBIG,
		sqlw::status::Code::CONSTRAINT,
		sqlw::status::Code::MISMATCH,
		sqlw::status::Code::MISUSE,
		sqlw::status::Code::NOLFS,
		sqlw::status::Code::AUTH,
		sqlw::status::Code::FORMAT,
		sqlw::status::Code::RANGE,
		sqlw::status::Code::NOTADB,
		sqlw::status::Code::NOTICE,
		sqlw::status::Code::WARNING,
		sqlw::status::Code::ROW,
		sqlw::status::Code::DONE,
		sqlw::status::Code::OK_LOAD_PERMANENTLY,
		sqlw::status::Code::ERROR_MISSING_COLLSEQ,
		sqlw::status::Code::BUSY_RECOVERY,
		sqlw::status::Code::LOCKED_SHAREDCACHE,
		sqlw::status::Code::READONLY_RECOVERY,
		sqlw::status::Code::IOERR_READ,
		sqlw::status::Code::CORRUPT_VTAB,
		sqlw::status::Code::CANTOPEN_NOTEMPDIR,
		sqlw::status::Code::CONSTRAINT_CHECK,
		sqlw::status::Code::AUTH_USER,
		sqlw::status::Code::NOTICE_RECOVER_WAL,
		sqlw::status::Code::WARNING_AUTOINDEX,
		sqlw::status::Code::ERROR_RETRY,
		sqlw::status::Code::ABORT_ROLLBACK,
		sqlw::status::Code::BUSY_SNAPSHOT,
		sqlw::status::Code::LOCKED_VTAB,
		sqlw::status::Code::READONLY_CANTLOCK,
		sqlw::status::Code::IOERR_SHORT_READ,
		sqlw::status::Code::CORRUPT_SEQUENCE,
		sqlw::status::Code::CANTOPEN_ISDIR,
		sqlw::status::Code::CONSTRAINT_COMMITHOOK,
		sqlw::status::Code::NOTICE_RECOVER_ROLLBACK,
		sqlw::status::Code::ERROR_SNAPSHOT,
		sqlw::status::Code::BUSY_TIMEOUT,
		sqlw::status::Code::READONLY_ROLLBACK,
		sqlw::status::Code::IOERR_WRITE,
		sqlw::status::Code::CORRUPT_INDEX,
		sqlw::status::Code::CANTOPEN_FULLPATH,
		sqlw::status::Code::CONSTRAINT_FOREIGNKEY,
		sqlw::status::Code::READONLY_DBMOVED,
		sqlw::status::Code::IOERR_FSYNC,
		sqlw::status::Code::CANTOPEN_CONVPATH,
		sqlw::status::Code::CONSTRAINT_FUNCTION,
		sqlw::status::Code::READONLY_CANTINIT,
		sqlw::status::Code::IOERR_DIR_FSYNC,
		sqlw::status::Code::CANTOPEN_DIRTYWAL,
		sqlw::status::Code::CONSTRAINT_NOTNULL,
		sqlw::status::Code::READONLY_DIRECTORY,
		sqlw::status::Code::IOERR_TRUNCATE,
		sqlw::status::Code::CANTOPEN_SYMLINK,
		sqlw::status::Code::CONSTRAINT_PRIMARYKEY,
		sqlw::status::Code::IOERR_FSTAT,
		sqlw::status::Code::CONSTRAINT_TRIGGER,
		sqlw::status::Code::IOERR_UNLOCK,
		sqlw::status::Code::CONSTRAINT_UNIQUE,
		sqlw::status::Code::IOERR_RDLOCK,
		sqlw::status::Code::CONSTRAINT_VTAB,
		sqlw::status::Code::IOERR_DELETE,
		sqlw::status::Code::CONSTRAINT_ROWID,
		sqlw::status::Code::IOERR_BLOCKED,
		sqlw::status::Code::CONSTRAINT_PINNED,
		sqlw::status::Code::IOERR_NOMEM,
		sqlw::status::Code::CONSTRAINT_DATATYPE,
		sqlw::status::Code::IOERR_ACCESS,
		sqlw::status::Code::IOERR_CHECKRESERVEDLOCK,
		sqlw::status::Code::IOERR_LOCK,
		sqlw::status::Code::IOERR_CLOSE,
		sqlw::status::Code::IOERR_DIR_CLOSE,
		sqlw::status::Code::IOERR_SHMOPEN,
		sqlw::status::Code::IOERR_SHMSIZE,
		sqlw::status::Code::IOERR_SHMLOCK,
		sqlw::status::Code::IOERR_SHMMAP,
		sqlw::status::Code::IOERR_SEEK,
		sqlw::status::Code::IOERR_DELETE_NOENT,
		sqlw::status::Code::IOERR_MMAP,
		sqlw::status::Code::IOERR_GETTEMPPATH,
		sqlw::status::Code::IOERR_CONVPATH,
		sqlw::status::Code::IOERR_VNODE,
		sqlw::status::Code::IOERR_AUTH,
		sqlw::status::Code::IOERR_BEGIN_ATOMIC,
		sqlw::status::Code::IOERR_COMMIT_ATOMIC,
		sqlw::status::Code::IOERR_ROLLBACK_ATOMIC,
		sqlw::status::Code::IOERR_DATA,
		sqlw::status::Code::IOERR_CORRUPTFS,
	};

	for(auto i : codes)
	{
		const auto msg = sqlw::status::verbose(i);

		std::cout << msg << '\n';
	}

	std::cout << "---END PRINTING POSSIBLE SQLITE ERROR CODES---\n";
}
