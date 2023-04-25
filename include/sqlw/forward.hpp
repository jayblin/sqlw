#ifndef SQLW_FORWARD_H_
#define SQLW_FORWARD_H_

#include "sqlite3.h"
#include <string>

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

	namespace status
	{
		/**
		 * @see https://www.sqlite.org/rescode.html
		 */
		enum class Code
		{
			/* not an error */
			SQLW_OK = 0,
			/* SQL logic error */
			SQLW_ERROR = 1,
			/* unknown error */
			SQLW_INTERNAL = 2,
			/* access permission denied */
			SQLW_PERM = 3,
			/* query aborted */
			SQLW_ABORT = 4,
			/* database is locked */
			SQLW_BUSY = 5,
			/* database table is locked */
			SQLW_LOCKED = 6,
			/* out of memory */
			SQLW_NOMEM = 7,
			/* attempt to write a readonly database */
			SQLW_READONLY = 8,
			/* interrupted */
			SQLW_INTERRUPT = 9,
			/* disk I/O error */
			SQLW_IOERR = 10,
			/* database disk image is malformed */
			SQLW_CORRUPT = 11,
			/* unknown operation */
			SQLW_NOTFOUND = 12,
			/* database or disk is full */
			SQLW_FULL = 13,
			/* unable to open database file */
			SQLW_CANTOPEN = 14,
			/* locking protocol */
			SQLW_PROTOCOL = 15,
			/* unknown error */
			SQLW_EMPTY = 16,
			/* database schema has changed */
			SQLW_SCHEMA = 17,
			/* string or blob too big */
			SQLW_TOOBIG = 18,
			/* constraint failed */
			SQLW_CONSTRAINT = 19,
			/* datatype mismatch */
			SQLW_MISMATCH = 20,
			/* bad parameter or other API misuse */
			SQLW_MISUSE = 21,
			/* unknown error */
			SQLW_NOLFS = 22,
			/* authorization denied */
			SQLW_AUTH = 23,
			/* unknown error */
			SQLW_FORMAT = 24,
			/* column index out of range */
			SQLW_RANGE = 25,
			/* file is not a database */
			SQLW_NOTADB = 26,
			/* notification message */
			SQLW_NOTICE = 27,
			/* warning message */
			SQLW_WARNING = 28,
			/* another row available */
			SQLW_ROW = 100,
			/* no more rows available */
			SQLW_DONE = 101,
			/* not an error */
			SQLW_OK_LOAD_PERMANENTLY = 256,
			/* SQL logic error */
			SQLW_ERROR_MISSING_COLLSEQ = 257,
			/* database is locked */
			SQLW_BUSY_RECOVERY = 261,
			/* database table is locked */
			SQLW_LOCKED_SHAREDCACHE = 262,
			/* attempt to write a readonly database */
			SQLW_READONLY_RECOVERY = 264,
			/* disk I/O error */
			SQLW_IOERR_READ = 266,
			/* database disk image is malformed */
			SQLW_CORRUPT_VTAB = 267,
			/* unable to open database file */
			SQLW_CANTOPEN_NOTEMPDIR = 270,
			/* constraint failed */
			SQLW_CONSTRAINT_CHECK = 275,
			/* authorization denied */
			SQLW_AUTH_USER = 279,
			/* notification message */
			SQLW_NOTICE_RECOVER_WAL = 283,
			/* warning message */
			SQLW_WARNING_AUTOINDEX = 284,
			/* SQL logic error */
			SQLW_ERROR_RETRY = 513,
			/* abort due to ROLLBACK */
			SQLW_ABORT_ROLLBACK = 516,
			/* database is locked */
			SQLW_BUSY_SNAPSHOT = 517,
			/* database table is locked */
			SQLW_LOCKED_VTAB = 518,
			/* attempt to write a readonly database */
			SQLW_READONLY_CANTLOCK = 520,
			/* disk I/O error */
			SQLW_IOERR_SHORT_READ = 522,
			/* database disk image is malformed */
			SQLW_CORRUPT_SEQUENCE = 523,
			/* unable to open database file */
			SQLW_CANTOPEN_ISDIR = 526,
			/* constraint failed */
			SQLW_CONSTRAINT_COMMITHOOK = 531,
			/* notification message */
			SQLW_NOTICE_RECOVER_ROLLBACK = 539,
			/* SQL logic error */
			SQLW_ERROR_SNAPSHOT = 769,
			/* database is locked */
			SQLW_BUSY_TIMEOUT = 773,
			/* attempt to write a readonly database */
			SQLW_READONLY_ROLLBACK = 776,
			/* disk I/O error */
			SQLW_IOERR_WRITE = 778,
			/* database disk image is malformed */
			SQLW_CORRUPT_INDEX = 779,
			/* unable to open database file */
			SQLW_CANTOPEN_FULLPATH = 782,
			/* constraint failed */
			SQLW_CONSTRAINT_FOREIGNKEY = 787,
			/* attempt to write a readonly database */
			SQLW_READONLY_DBMOVED = 1032,
			/* disk I/O error */
			SQLW_IOERR_FSYNC = 1034,
			/* unable to open database file */
			SQLW_CANTOPEN_CONVPATH = 1038,
			/* constraint failed */
			SQLW_CONSTRAINT_FUNCTION = 1043,
			/* attempt to write a readonly database */
			SQLW_READONLY_CANTINIT = 1288,
			/* disk I/O error */
			SQLW_IOERR_DIR_FSYNC = 1290,
			/* unable to open database file */
			SQLW_CANTOPEN_DIRTYWAL = 1294,
			/* constraint failed */
			SQLW_CONSTRAINT_NOTNULL = 1299,
			/* attempt to write a readonly database */
			SQLW_READONLY_DIRECTORY = 1544,
			/* disk I/O error */
			SQLW_IOERR_TRUNCATE = 1546,
			/* unable to open database file */
			SQLW_CANTOPEN_SYMLINK = 1550,
			/* constraint failed */
			SQLW_CONSTRAINT_PRIMARYKEY = 1555,
			/* disk I/O error */
			SQLW_IOERR_FSTAT = 1802,
			/* constraint failed */
			SQLW_CONSTRAINT_TRIGGER = 1811,
			/* disk I/O error */
			SQLW_IOERR_UNLOCK = 2058,
			/* constraint failed */
			SQLW_CONSTRAINT_UNIQUE = 2067,
			/* disk I/O error */
			SQLW_IOERR_RDLOCK = 2314,
			/* constraint failed */
			SQLW_CONSTRAINT_VTAB = 2323,
			/* disk I/O error */
			SQLW_IOERR_DELETE = 2570,
			/* constraint failed */
			SQLW_CONSTRAINT_ROWID = 2579,
			/* disk I/O error */
			SQLW_IOERR_BLOCKED = 2826,
			/* constraint failed */
			SQLW_CONSTRAINT_PINNED = 2835,
			/* disk I/O error */
			SQLW_IOERR_NOMEM = 3082,
			/* constraint failed */
			SQLW_CONSTRAINT_DATATYPE = 3091,
			/* disk I/O error */
			SQLW_IOERR_ACCESS = 3338,
			/* disk I/O error */
			SQLW_IOERR_CHECKRESERVEDLOCK = 3594,
			/* disk I/O error */
			SQLW_IOERR_LOCK = 3850,
			/* disk I/O error */
			SQLW_IOERR_CLOSE = 4106,
			/* disk I/O error */
			SQLW_IOERR_DIR_CLOSE = 4362,
			/* disk I/O error */
			SQLW_IOERR_SHMOPEN = 4618,
			/* disk I/O error */
			SQLW_IOERR_SHMSIZE = 4874,
			/* disk I/O error */
			SQLW_IOERR_SHMLOCK = 5130,
			/* disk I/O error */
			SQLW_IOERR_SHMMAP = 5386,
			/* disk I/O error */
			SQLW_IOERR_SEEK = 5642,
			/* disk I/O error */
			SQLW_IOERR_DELETE_NOENT = 5898,
			/* disk I/O error */
			SQLW_IOERR_MMAP = 6154,
			/* disk I/O error */
			SQLW_IOERR_GETTEMPPATH = 6410,
			/* disk I/O error */
			SQLW_IOERR_CONVPATH = 6666,
			/* disk I/O error */
			SQLW_IOERR_VNODE = 6922,
			/* disk I/O error */
			SQLW_IOERR_AUTH = 7178,
			/* disk I/O error */
			SQLW_IOERR_BEGIN_ATOMIC = 7434,
			/* disk I/O error */
			SQLW_IOERR_COMMIT_ATOMIC = 7690,
			/* disk I/O error */
			SQLW_IOERR_ROLLBACK_ATOMIC = 7946,
			/* disk I/O error */
			SQLW_IOERR_DATA = 8202,
			/* disk I/O error */
			SQLW_IOERR_CORRUPTFS = 8458,
			_CLOSED_ = 9999,
		};
	} // namespace status
} // namespace sqlw

#endif // SQLW_FORWARD_H_
