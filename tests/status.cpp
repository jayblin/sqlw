#include "sqlw/forward.hpp"
#include <gtest/gtest.h>
#include <system_error>

TEST(Status, check_condition_comparison_to_sqlite_status_codes)
{
    std::error_code ec = sqlw::status::Code{SQLITE_OK};
    std::error_condition cond = sqlw::status::Condition::OK;
    ASSERT_TRUE(ec == cond);

    ec = sqlw::status::Code{SQLITE_DONE};
    cond = sqlw::status::Condition::OK;
    ASSERT_TRUE(ec == cond);

    ec = sqlw::status::Code{SQLITE_ROW};
    cond = sqlw::status::Condition::OK;
    ASSERT_TRUE(ec == cond);

    ec = sqlw::status::Code{SQLITE_DONE};
    cond = sqlw::status::Condition::DONE;
    ASSERT_TRUE(ec == cond);

    ec = sqlw::status::Code{SQLITE_ROW};
    cond = sqlw::status::Condition::ROW;
    ASSERT_TRUE(ec == cond);

    ec = sqlw::status::Code{SQLITE_ROW};
    cond = sqlw::status::Condition::DONE;
    ASSERT_TRUE(ec != cond);

    ec = sqlw::status::Code{SQLITE_DONE};
    cond = sqlw::status::Condition::ROW;
    ASSERT_TRUE(ec != cond);

    ec = sqlw::status::Code::CLOSED_HANDLE;
    cond = sqlw::status::Condition::CLOSED_HANDLE;
    ASSERT_TRUE(ec == cond);

    ec = sqlw::status::Code{SQLITE_IOERR_CLOSE};
    cond = sqlw::status::Condition::ERROR;
    ASSERT_TRUE(ec == cond);
}
