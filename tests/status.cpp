#include "sqlw/status.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include <cstdio>
#include <gtest/gtest.h>
#include <vector>

TEST(Status, can_determine_ok_status)
{
	ASSERT_TRUE(sqlw::status::is_ok(sqlw::status::Code::SQLW_OK));
	ASSERT_TRUE(sqlw::status::is_ok(sqlw::status::Code::SQLW_DONE));
	ASSERT_TRUE(sqlw::status::is_ok(sqlw::status::Code::SQLW_ROW));
}

TEST(Status, can_report_readable_error)
{
	std::cout << "---PRINTING POSSIBLE SQLITE ERROR CODES---\n";

	std::vector<sqlw::status::Code> codes = {
	    sqlw::status::Code::SQLW_OK,
	    sqlw::status::Code::SQLW_ERROR,
	    sqlw::status::Code::SQLW_INTERNAL,
	    sqlw::status::Code::SQLW_PERM,
	    sqlw::status::Code::SQLW_ABORT,
	    sqlw::status::Code::SQLW_BUSY,
	    sqlw::status::Code::SQLW_LOCKED,
	    sqlw::status::Code::SQLW_NOMEM,
	    sqlw::status::Code::SQLW_READONLY,
	    sqlw::status::Code::SQLW_INTERRUPT,
	    sqlw::status::Code::SQLW_IOERR,
	    sqlw::status::Code::SQLW_CORRUPT,
	    sqlw::status::Code::SQLW_NOTFOUND,
	    sqlw::status::Code::SQLW_FULL,
	    sqlw::status::Code::SQLW_CANTOPEN,
	    sqlw::status::Code::SQLW_PROTOCOL,
	    sqlw::status::Code::SQLW_EMPTY,
	    sqlw::status::Code::SQLW_SCHEMA,
	    sqlw::status::Code::SQLW_TOOBIG,
	    sqlw::status::Code::SQLW_CONSTRAINT,
	    sqlw::status::Code::SQLW_MISMATCH,
	    sqlw::status::Code::SQLW_MISUSE,
	    sqlw::status::Code::SQLW_NOLFS,
	    sqlw::status::Code::SQLW_AUTH,
	    sqlw::status::Code::SQLW_FORMAT,
	    sqlw::status::Code::SQLW_RANGE,
	    sqlw::status::Code::SQLW_NOTADB,
	    sqlw::status::Code::SQLW_NOTICE,
	    sqlw::status::Code::SQLW_WARNING,
	    sqlw::status::Code::SQLW_ROW,
	    sqlw::status::Code::SQLW_DONE,
	    sqlw::status::Code::SQLW_OK_LOAD_PERMANENTLY,
	    sqlw::status::Code::SQLW_ERROR_MISSING_COLLSEQ,
	    sqlw::status::Code::SQLW_BUSY_RECOVERY,
	    sqlw::status::Code::SQLW_LOCKED_SHAREDCACHE,
	    sqlw::status::Code::SQLW_READONLY_RECOVERY,
	    sqlw::status::Code::SQLW_IOERR_READ,
	    sqlw::status::Code::SQLW_CORRUPT_VTAB,
	    sqlw::status::Code::SQLW_CANTOPEN_NOTEMPDIR,
	    sqlw::status::Code::SQLW_CONSTRAINT_CHECK,
	    sqlw::status::Code::SQLW_AUTH_USER,
	    sqlw::status::Code::SQLW_NOTICE_RECOVER_WAL,
	    sqlw::status::Code::SQLW_WARNING_AUTOINDEX,
	    sqlw::status::Code::SQLW_ERROR_RETRY,
	    sqlw::status::Code::SQLW_ABORT_ROLLBACK,
	    sqlw::status::Code::SQLW_BUSY_SNAPSHOT,
	    sqlw::status::Code::SQLW_LOCKED_VTAB,
	    sqlw::status::Code::SQLW_READONLY_CANTLOCK,
	    sqlw::status::Code::SQLW_IOERR_SHORT_READ,
	    sqlw::status::Code::SQLW_CORRUPT_SEQUENCE,
	    sqlw::status::Code::SQLW_CANTOPEN_ISDIR,
	    sqlw::status::Code::SQLW_CONSTRAINT_COMMITHOOK,
	    sqlw::status::Code::SQLW_NOTICE_RECOVER_ROLLBACK,
	    sqlw::status::Code::SQLW_ERROR_SNAPSHOT,
	    sqlw::status::Code::SQLW_BUSY_TIMEOUT,
	    sqlw::status::Code::SQLW_READONLY_ROLLBACK,
	    sqlw::status::Code::SQLW_IOERR_WRITE,
	    sqlw::status::Code::SQLW_CORRUPT_INDEX,
	    sqlw::status::Code::SQLW_CANTOPEN_FULLPATH,
	    sqlw::status::Code::SQLW_CONSTRAINT_FOREIGNKEY,
	    sqlw::status::Code::SQLW_READONLY_DBMOVED,
	    sqlw::status::Code::SQLW_IOERR_FSYNC,
	    sqlw::status::Code::SQLW_CANTOPEN_CONVPATH,
	    sqlw::status::Code::SQLW_CONSTRAINT_FUNCTION,
	    sqlw::status::Code::SQLW_READONLY_CANTINIT,
	    sqlw::status::Code::SQLW_IOERR_DIR_FSYNC,
	    sqlw::status::Code::SQLW_CANTOPEN_DIRTYWAL,
	    sqlw::status::Code::SQLW_CONSTRAINT_NOTNULL,
	    sqlw::status::Code::SQLW_READONLY_DIRECTORY,
	    sqlw::status::Code::SQLW_IOERR_TRUNCATE,
	    sqlw::status::Code::SQLW_CANTOPEN_SYMLINK,
	    sqlw::status::Code::SQLW_CONSTRAINT_PRIMARYKEY,
	    sqlw::status::Code::SQLW_IOERR_FSTAT,
	    sqlw::status::Code::SQLW_CONSTRAINT_TRIGGER,
	    sqlw::status::Code::SQLW_IOERR_UNLOCK,
	    sqlw::status::Code::SQLW_CONSTRAINT_UNIQUE,
	    sqlw::status::Code::SQLW_IOERR_RDLOCK,
	    sqlw::status::Code::SQLW_CONSTRAINT_VTAB,
	    sqlw::status::Code::SQLW_IOERR_DELETE,
	    sqlw::status::Code::SQLW_CONSTRAINT_ROWID,
	    sqlw::status::Code::SQLW_IOERR_BLOCKED,
	    sqlw::status::Code::SQLW_CONSTRAINT_PINNED,
	    sqlw::status::Code::SQLW_IOERR_NOMEM,
	    sqlw::status::Code::SQLW_CONSTRAINT_DATATYPE,
	    sqlw::status::Code::SQLW_IOERR_ACCESS,
	    sqlw::status::Code::SQLW_IOERR_CHECKRESERVEDLOCK,
	    sqlw::status::Code::SQLW_IOERR_LOCK,
	    sqlw::status::Code::SQLW_IOERR_CLOSE,
	    sqlw::status::Code::SQLW_IOERR_DIR_CLOSE,
	    sqlw::status::Code::SQLW_IOERR_SHMOPEN,
	    sqlw::status::Code::SQLW_IOERR_SHMSIZE,
	    sqlw::status::Code::SQLW_IOERR_SHMLOCK,
	    sqlw::status::Code::SQLW_IOERR_SHMMAP,
	    sqlw::status::Code::SQLW_IOERR_SEEK,
	    sqlw::status::Code::SQLW_IOERR_DELETE_NOENT,
	    sqlw::status::Code::SQLW_IOERR_MMAP,
	    sqlw::status::Code::SQLW_IOERR_GETTEMPPATH,
	    sqlw::status::Code::SQLW_IOERR_CONVPATH,
	    sqlw::status::Code::SQLW_IOERR_VNODE,
	    sqlw::status::Code::SQLW_IOERR_AUTH,
	    sqlw::status::Code::SQLW_IOERR_BEGIN_ATOMIC,
	    sqlw::status::Code::SQLW_IOERR_COMMIT_ATOMIC,
	    sqlw::status::Code::SQLW_IOERR_ROLLBACK_ATOMIC,
	    sqlw::status::Code::SQLW_IOERR_DATA,
	    sqlw::status::Code::SQLW_IOERR_CORRUPTFS,
	};

	for (auto i : codes)
	{
		const auto msg = sqlw::status::verbose(i);

		std::cout << msg << '\n';
	}

	std::cout << "---END PRINTING POSSIBLE SQLITE ERROR CODES---\n";
}
