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
			OK = 0,
			/* SQL logic error */
			SQLW_ERROR = 1,
			/* unknown error */
			INTERNAL = 2,
			/* access permission denied */
			PERM = 3,
			/* query aborted */
			ABORT = 4,
			/* database is locked */
			BUSY = 5,
			/* database table is locked */
			LOCKED = 6,
			/* out of memory */
			NOMEM = 7,
			/* attempt to write a readonly database */
			READONLY = 8,
			/* interrupted */
			INTERRUPT = 9,
			/* disk I/O error */
			IOERR = 10,
			/* database disk image is malformed */
			CORRUPT = 11,
			/* unknown operation */
			NOTFOUND = 12,
			/* database or disk is full */
			FULL = 13,
			/* unable to open database file */
			CANTOPEN = 14,
			/* locking protocol */
			PROTOCOL = 15,
			/* unknown error */
			EMPTY = 16,
			/* database schema has changed */
			SCHEMA = 17,
			/* string or blob too big */
			TOOBIG = 18,
			/* constraint failed */
			CONSTRAINT = 19,
			/* datatype mismatch */
			MISMATCH = 20,
			/* bad parameter or other API misuse */
			MISUSE = 21,
			/* unknown error */
			NOLFS = 22,
			/* authorization denied */
			AUTH = 23,
			/* unknown error */
			FORMAT = 24,
			/* column index out of range */
			RANGE = 25,
			/* file is not a database */
			NOTADB = 26,
			/* notification message */
			NOTICE = 27,
			/* warning message */
			WARNING = 28,
			/* another row available */
			ROW = 100,
			/* no more rows available */
			DONE = 101,
			/* not an error */
			OK_LOAD_PERMANENTLY = 256,
			/* SQL logic error */
			ERROR_MISSING_COLLSEQ = 257,
			/* database is locked */
			BUSY_RECOVERY = 261,
			/* database table is locked */
			LOCKED_SHAREDCACHE = 262,
			/* attempt to write a readonly database */
			READONLY_RECOVERY = 264,
			/* disk I/O error */
			IOERR_READ = 266,
			/* database disk image is malformed */
			CORRUPT_VTAB = 267,
			/* unable to open database file */
			CANTOPEN_NOTEMPDIR = 270,
			/* constraint failed */
			CONSTRAINT_CHECK = 275,
			/* authorization denied */
			AUTH_USER = 279,
			/* notification message */
			NOTICE_RECOVER_WAL = 283,
			/* warning message */
			WARNING_AUTOINDEX = 284,
			/* SQL logic error */
			ERROR_RETRY = 513,
			/* abort due to ROLLBACK */
			ABORT_ROLLBACK = 516,
			/* database is locked */
			BUSY_SNAPSHOT = 517,
			/* database table is locked */
			LOCKED_VTAB = 518,
			/* attempt to write a readonly database */
			READONLY_CANTLOCK = 520,
			/* disk I/O error */
			IOERR_SHORT_READ = 522,
			/* database disk image is malformed */
			CORRUPT_SEQUENCE = 523,
			/* unable to open database file */
			CANTOPEN_ISDIR = 526,
			/* constraint failed */
			CONSTRAINT_COMMITHOOK = 531,
			/* notification message */
			NOTICE_RECOVER_ROLLBACK = 539,
			/* SQL logic error */
			ERROR_SNAPSHOT = 769,
			/* database is locked */
			BUSY_TIMEOUT = 773,
			/* attempt to write a readonly database */
			READONLY_ROLLBACK = 776,
			/* disk I/O error */
			IOERR_WRITE = 778,
			/* database disk image is malformed */
			CORRUPT_INDEX = 779,
			/* unable to open database file */
			CANTOPEN_FULLPATH = 782,
			/* constraint failed */
			CONSTRAINT_FOREIGNKEY = 787,
			/* attempt to write a readonly database */
			READONLY_DBMOVED = 1032,
			/* disk I/O error */
			IOERR_FSYNC = 1034,
			/* unable to open database file */
			CANTOPEN_CONVPATH = 1038,
			/* constraint failed */
			CONSTRAINT_FUNCTION = 1043,
			/* attempt to write a readonly database */
			READONLY_CANTINIT = 1288,
			/* disk I/O error */
			IOERR_DIR_FSYNC = 1290,
			/* unable to open database file */
			CANTOPEN_DIRTYWAL = 1294,
			/* constraint failed */
			CONSTRAINT_NOTNULL = 1299,
			/* attempt to write a readonly database */
			READONLY_DIRECTORY = 1544,
			/* disk I/O error */
			IOERR_TRUNCATE = 1546,
			/* unable to open database file */
			CANTOPEN_SYMLINK = 1550,
			/* constraint failed */
			CONSTRAINT_PRIMARYKEY = 1555,
			/* disk I/O error */
			IOERR_FSTAT = 1802,
			/* constraint failed */
			CONSTRAINT_TRIGGER = 1811,
			/* disk I/O error */
			IOERR_UNLOCK = 2058,
			/* constraint failed */
			CONSTRAINT_UNIQUE = 2067,
			/* disk I/O error */
			IOERR_RDLOCK = 2314,
			/* constraint failed */
			CONSTRAINT_VTAB = 2323,
			/* disk I/O error */
			IOERR_DELETE = 2570,
			/* constraint failed */
			CONSTRAINT_ROWID = 2579,
			/* disk I/O error */
			IOERR_BLOCKED = 2826,
			/* constraint failed */
			CONSTRAINT_PINNED = 2835,
			/* disk I/O error */
			IOERR_NOMEM = 3082,
			/* constraint failed */
			CONSTRAINT_DATATYPE = 3091,
			/* disk I/O error */
			IOERR_ACCESS = 3338,
			/* disk I/O error */
			IOERR_CHECKRESERVEDLOCK = 3594,
			/* disk I/O error */
			IOERR_LOCK = 3850,
			/* disk I/O error */
			IOERR_CLOSE = 4106,
			/* disk I/O error */
			IOERR_DIR_CLOSE = 4362,
			/* disk I/O error */
			IOERR_SHMOPEN = 4618,
			/* disk I/O error */
			IOERR_SHMSIZE = 4874,
			/* disk I/O error */
			IOERR_SHMLOCK = 5130,
			/* disk I/O error */
			IOERR_SHMMAP = 5386,
			/* disk I/O error */
			IOERR_SEEK = 5642,
			/* disk I/O error */
			IOERR_DELETE_NOENT = 5898,
			/* disk I/O error */
			IOERR_MMAP = 6154,
			/* disk I/O error */
			IOERR_GETTEMPPATH = 6410,
			/* disk I/O error */
			IOERR_CONVPATH = 6666,
			/* disk I/O error */
			IOERR_VNODE = 6922,
			/* disk I/O error */
			IOERR_AUTH = 7178,
			/* disk I/O error */
			IOERR_BEGIN_ATOMIC = 7434,
			/* disk I/O error */
			IOERR_COMMIT_ATOMIC = 7690,
			/* disk I/O error */
			IOERR_ROLLBACK_ATOMIC = 7946,
			/* disk I/O error */
			IOERR_DATA = 8202,
			/* disk I/O error */
			IOERR_CORRUPTFS = 8458,
			_CLOSED_ = 9999,
		};
	} // namespace status
} // namespace sqlw

#endif // SQLW_FORWARD_H_
