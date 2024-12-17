#include "sqlw/transaction.hpp"
#include "sqlw/statement.hpp"
#include "sqlw/connection.hpp"
#include "sqlw/forward.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <system_error>
#include <tuple>

static void errorLogCallback(void* _, int iErrCode, const char* zMsg)
{
    std::cout << '[' << iErrCode << "] " << zMsg << '\n';
}

typedef std::tuple<
    std::string_view,                                   // case_name
    std::function<std::error_code(sqlw::Transaction&)>, // testee
    sqlw::status::Condition,                            // expected_return_code
    std::string_view                                    // expected_state
    >
    test_case_t;

class TransactionTest : public testing::TestWithParam<test_case_t>
{
  public:
    static void SetUpTestSuite()
    {
        sqlite3_config(SQLITE_CONFIG_LOG, errorLogCallback, nullptr);

        db_con = {":memory:"};

        sqlw::Statement stmt = {&db_con};
        std::error_code ec = stmt(R"(CREATE TABLE user (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE
        ))");
        ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;
        ASSERT_EQ(ec, stmt.status());
    }

    static void TearDownTestSuite()
    {
    }

  protected:
    static sqlw::Connection db_con;

    void SetUp() override
    {
        sqlw::Statement stmt = {&db_con};

        auto ec = stmt("SAVEPOINT unittestsp");
        ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;
        ASSERT_EQ(ec, stmt.status());
    }

    void TearDown() override
    {
        auto stmt = sqlw::Statement{&db_con};
        std::error_code ec = stmt("ROLLBACK TO unittestsp");
        ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;
        ASSERT_EQ(ec, stmt.status());
    }
};
sqlw::Connection TransactionTest::db_con;

TEST_P(TransactionTest, can_be_invoked)
{
    const auto [_, testee, expected_return_code, expected_state] = GetParam();

    sqlw::Statement stmt{&db_con};
    std::error_code ec = stmt.status();
    ASSERT_TRUE(sqlw::status::Condition::OK == ec) << ec;
    ASSERT_EQ(ec, stmt.status());

    ec = stmt("INSERT INTO user (id, name) VALUES (1,'kate'),(2,'eris')");
    ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;
    ASSERT_EQ(ec, stmt.status());

    sqlw::Transaction t{&db_con};
    ec = testee(t);
    ASSERT_TRUE(expected_return_code == ec) << ec;

    std::stringstream ss;
    ec = stmt("SELECT * FROM user", [&](sqlw::Statement::ExecArgs args) {
        ss << args.column_value << ",";
    });
    ASSERT_TRUE(sqlw::status::Condition::DONE == ec) << ec;
    ASSERT_EQ(ec, stmt.status());
    ASSERT_STREQ(expected_state.data(), ss.str().data());
}

INSTANTIATE_TEST_SUITE_P(
    Instantiantion,
    TransactionTest,
    testing::Values(
        // Passing just the sql string.
        test_case_t{
            "A",
            [](sqlw::Transaction& t) {
                return t("INSERT INTO user (id, name) VALUES (3,'bob')");
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,bob,",
        },
        // Passing just the sql string. Error in sql.
        test_case_t{
            "A0",
            [](sqlw::Transaction& t) {
                return t("INSERT INTO user (id, ) VALUES (3,'bob')");
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,",
        },
        // Passing just the sql string. Multiple statements to execute.
        test_case_t{
            "A1",
            [](sqlw::Transaction& t) {
                return t("INSERT INTO user (id, name) VALUES (3,'bob'); "
                         "UPDATE user SET name='Tony' WHERE id = 3");
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Tony,",
        },
        // Passing just the sql string. Multiple statements to execute. Second
        // causes an error.
        test_case_t{
            "A2",
            [](sqlw::Transaction& t) {
                return t("INSERT INTO user (id, name) VALUES (3,'bob'); "
                         "UPDATE user SET name='Tony' WHERE id = 10");
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,bob,",
        },
        // Passing just the sql string. Causes an error because of INSERTing
        // same entity.
        test_case_t{
            "A3",
            [](sqlw::Transaction& t) {
                return t("INSERT INTO user (id, name) VALUES (2,'bob')");
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,",
        },
        // Passing just the sql string. Multiple statements. Third one cases a
        // fatal error.
        test_case_t{
            "A4",
            [](sqlw::Transaction& t) {
                return t("INSERT INTO user (id, name) VALUES (3,'bob');"
                         "INSERT INTO user (id, name) VALUES (4,'john');"
                         "INSERT INTO user (id, uiui) VALUES (5,'ronda')");
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,",
        },
        // Passing sql string + array of parameters to bind.
        test_case_t{
            "B",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned')",
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,"},
        // Passing sql string + array of parameters to bind. Error in sql.
        test_case_t{
            "B0",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) _VALUES (3,?1),(?2, 'Ned')",
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + array of parameters to bind. Bad parameter.
        test_case_t{
            "B1",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned')",
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"", sqlw::Type::SQL_INT},
                    }});
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + array of parameters to bind. Multiple
        // statements.
        test_case_t{
            "B2",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned'); "
                    "UPDATE user SET name='Neddy' WHERE id=6",
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Neddy,"},
        // Passing sql string + array of parameters to bind. Multiple
        // statements. Error in second statement.
        test_case_t{
            "B3",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned'); "
                    "UPDATE user SET name='Neddy' WHERE id=6666",
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,"},
        // Passing sql string + array of parameters to bind. Multiple
        // statements. Second statement has a parameter to bind. Note the use of
        // a colon in parameter templates! This is the way if you want to bind
        // parameters for multiple queries.
        test_case_t{
            "B4",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=:3; "
                    "UPDATE user SET name='Bobby' WHERE id=:4; "
                    "UPDATE user SET name='Bobster' WHERE name=:4",
                    std::array<sqlw::Statement::bindable_t, 5>{
                        {{"Bob", sqlw::Type::SQL_TEXT},
                         {"6", sqlw::Type::SQL_INT},
                         {"2", sqlw::Type::SQL_INT},
                         {"3", sqlw::Type::SQL_INT},
                         {"Bobby", sqlw::Type::SQL_TEXT}}});
            },
            sqlw::status::Condition::OK,
            "1,kate,2,Neddy,3,Bobster,6,Ned,"},
        // Passing sql string + array of parameters to bind. Multiple
        // statements. Error with a paramter for second statement.
        test_case_t{
            "B5",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned'); "
                    "UPDATE user SET name='Neddy' WHERE id=:3",
                    std::array<sqlw::Statement::bindable_t, 3>{
                        {{"Bob", sqlw::Type::SQL_TEXT},
                         {"6", sqlw::Type::SQL_INT},
                         {"notanint", sqlw::Type::SQL_INT}}});
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + array of parameters to bind. Multiple
        // statements. Fatal error in third statement.
        test_case_t{
            "B6",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned'); "
                    "UPDATE user SET name='Neddy' WHERE id=6666;"
                    "UPDATE user SET ___name='Horus' WHERE id=1",
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + vector of parameters to bind.
        test_case_t{
            "C",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned')",
                    std::vector<sqlw::Statement::bindable_t>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,",
        },
        // Passing sql string + tuple of parameters to bind.
        test_case_t{
            "D",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned')",
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Goku", sqlw::Type::SQL_TEXT},
                        47});
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Goku,47,Ned,"},
        // Passing sql string + tuple of parameters to bind. Error in sql.
        test_case_t{
            "D0",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) _VALUES (3,?1),(?2, 'Ned')",
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        sqlw::Statement::bindable_t{"6", sqlw::Type::SQL_INT},
                    });
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind. Bad parameter.
        test_case_t{
            "D1",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned')",
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        sqlw::Statement::bindable_t{"", sqlw::Type::SQL_INT},
                    });
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind. Multiple
        // statements.
        test_case_t{
            "D2",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned'); "
                    "UPDATE user SET name='Neddy' WHERE id=6",
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                    });
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Neddy,"},
        // Passing sql string + tuple of parameters to bind. Multiple
        // statements. Error in second statement.
        test_case_t{
            "D3",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned'); "
                    "UPDATE user SET name='Neddy' WHERE id=6666",
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                    });
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,"},
        // Passing sql string + tuple of parameters to bind. Multiple
        // statements. Second statement has a parameter to bind. Will return
        // error as i am not smart enough to figure out how to pass remaining
        // params as a subtuple.
        test_case_t{
            "D4",
            [](sqlw::Transaction& t) {
                auto ec =
                    t("INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned');"
                      "UPDATE user SET name='Neddy' WHERE id=:3; "
                      "UPDATE user SET name='Bobby' WHERE id=:4; "
                      "UPDATE user SET name='Bobster' WHERE name=:4",
                      std::tuple{
                          sqlw::Statement::bindable_t{
                              "Bob", sqlw::Type::SQL_TEXT},
                          6,
                          2,
                          3,
                          sqlw::Statement::bindable_t{
                              "Bobby", sqlw::Type::SQL_TEXT}});

                ([&]() {
                    ASSERT_EQ(sqlw::status::Code::UNUSED_PARAMETERS_ERROR, ec);
                })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind. Multiple
        // statements. Error with a paramter for second statement.
        test_case_t{
            "D5",
            [](sqlw::Transaction& t) {
                auto ec =
                    t("INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned'); "
                      "UPDATE user SET name='Neddy' WHERE id=:3",
                      std::tuple{
                          sqlw::Statement::bindable_t{
                              "Bob", sqlw::Type::SQL_TEXT},
                          sqlw::Statement::bindable_t{"6", sqlw::Type::SQL_INT},
                          sqlw::Statement::bindable_t{
                              "notanint", sqlw::Type::SQL_INT}});

                ([&]() {
                    ASSERT_EQ(sqlw::status::Code::UNUSED_PARAMETERS_ERROR, ec);
                })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind. Multiple
        // statements. Fatal error in third statement.
        test_case_t{
            "D6",
            [](sqlw::Transaction& t) {
                return t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned'); "
                    "UPDATE user SET name='Neddy' WHERE id=6666;"
                    "UPDATE user SETe name='HORUS' WHERE id=1",
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                    });
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + callback.
        test_case_t{
            "E",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,'bob') RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    });
                ([&]() { ASSERT_STREQ("3,bob,", ss.str().data()); })();
                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,bob,"},
        // Passing sql string + callback. Error in statment.
        test_case_t{
            "E0",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec =
                    t("INSERT INTO user (id, name VALUES (3,'bob') RETURNING *",
                      [&](sqlw::Statement::ExecArgs e) {
                          ss << e.column_value << ",";
                      });
                ([&]() { ASSERT_STREQ("", ss.str().data()); })();
                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + callback. Multiple statements
        test_case_t{
            "E1",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec =
                    t("INSERT INTO user (id, name) VALUES (3, 'bob');"
                      "UPDATE user SET name = 'Bob' WHERE id = 3;"
                      "SELECT * FROM user WHERE id = 1",
                      [&](sqlw::Statement::ExecArgs e) {
                          ss << e.column_value << ",";
                      });
                ([&]() { ASSERT_STREQ("1,kate,", ss.str().data()); })();
                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,"},
        // Passing sql string + callback. Multiple statements. Error in third
        // statement.
        test_case_t{
            "E2",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec =
                    t("INSERT INTO user (id, name) VALUES (3, 'bob');"
                      "UPDATE user SET name = 'Bob' WHERE id = 3;"
                      "SELECT * FROM user WHEREre id = 1",
                      [&](sqlw::Statement::ExecArgs e) {
                          ss << e.column_value << ",";
                      });
                ([&]() { ASSERT_STREQ("", ss.str().data()); })();
                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + array of parameters to bind + calback.
        test_case_t{
            "F",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned') "
                    "RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});

                ([&]() { ASSERT_STREQ("3,Bob,6,Ned,", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,"},
        // Passing sql string + array of parameters to bind + callback. Error in
        // sql.
        test_case_t{
            "F0",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) _VALUES (3,?1),(?2, 'Ned')",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + array of parameters to bind + callback. Bad
        // parameter.
        test_case_t{
            "F1",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned')",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"", sqlw::Type::SQL_INT},
                    }});

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + array of parameters to bind + callback. Multiple
        // statements.
        test_case_t{
            "F2",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=6 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});

                ([&]() { ASSERT_STREQ("6,Neddy,", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Neddy,"},
        // Passing sql string + array of parameters to bind + callback. Multiple
        // statements. Error in second statement.
        test_case_t{
            "F3",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=6666 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,"},
        // Passing sql string + array of parameters to bind + callback. Multiple
        // Note the use of a colon in parameter templates! This is the
        // way if you want to bind parameters for multiple queries.
        test_case_t{
            "F4",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=:3; "
                    "UPDATE user SET name='Bobby' WHERE id=:4; "
                    "UPDATE user SET name='Bobster' WHERE name=:4 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 5>{
                        {{"Bob", sqlw::Type::SQL_TEXT},
                         {"6", sqlw::Type::SQL_INT},
                         {"2", sqlw::Type::SQL_INT},
                         {"3", sqlw::Type::SQL_INT},
                         {"Bobby", sqlw::Type::SQL_TEXT}}});

                ([&]() { ASSERT_STREQ("3,Bobster,", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,Neddy,3,Bobster,6,Ned,"},
        // Passing sql string + array of parameters to bind + callback. Multiple
        // statements. Error with a paramter for second statement.
        test_case_t{
            "F5",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=:3 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 3>{
                        {{"Bob", sqlw::Type::SQL_TEXT},
                         {"6", sqlw::Type::SQL_INT},
                         {"notanint", sqlw::Type::SQL_INT}}});

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + array of parameters to bind + callback. Multiple
        // statements. Fatal error in third statement.
        test_case_t{
            "F6",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=6666;"
                    "UPDATE user SET name=HORUS WHERE id=1 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::array<sqlw::Statement::bindable_t, 2>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + vector of parameters to bind + callback.
        test_case_t{
            "G",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned') "
                    "RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::vector<sqlw::Statement::bindable_t>{{
                        {"Bob", sqlw::Type::SQL_TEXT},
                        {"6", sqlw::Type::SQL_INT},
                    }});

                ([&]() { ASSERT_STREQ("3,Bob,6,Ned,", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,",
        },
        // Passing sql string + tuple of parameters to bind + callback.
        test_case_t{
            "H",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned') "
                    "RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Goku", sqlw::Type::SQL_TEXT},
                        47});

                ([&]() { ASSERT_STREQ("3,Goku,47,Ned,", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Goku,47,Ned,",
        },
        // Passing sql string + tuple of parameters to bind + callback. Error in
        // sql.
        test_case_t{
            "H0",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) _VALUES (3,?1),(?2, 'Ned') "
                    "RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                    });

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind + callback. Bad
        // parameter.
        test_case_t{
            "H1",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2, 'Ned') "
                    "RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        22.22,
                    });

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind + callback. Multiple
        // statements.
        test_case_t{
            "H2",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=6 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                    });

                ([&]() { ASSERT_STREQ("6,Neddy,", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Neddy,"},
        // Passing sql string + tuple of parameters to bind + callback. Multiple
        // statements. Error in second statement.
        test_case_t{
            "H3",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=6666 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                    });

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::OK,
            "1,kate,2,eris,3,Bob,6,Ned,"},
        // Passing sql string + tuple of parameters to bind + callback. Multiple
        // statements. Second statement has a parameter to bind. Will return
        // error as i am not smart enough to figure out how to pass remaining
        // params as a subtuple.
        test_case_t{
            "H4",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=:3; "
                    "UPDATE user SET name='Bobby' WHERE id=:4; "
                    "UPDATE user SET name='Bobster' WHERE name=:4 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                        2,
                        3,
                        sqlw::Statement::bindable_t{
                            "Bobby", sqlw::Type::SQL_TEXT}});

                ([&]() {
                    ASSERT_EQ(sqlw::status::Code::UNUSED_PARAMETERS_ERROR, ec);
                    ASSERT_STREQ("", ss.str().data());
                })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind + callback. Multiple
        // statements. Error with a paramter for second statement.
        test_case_t{
            "H5",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,:1),(:2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=:3 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        sqlw::Statement::bindable_t{"6", sqlw::Type::SQL_INT},
                        sqlw::Statement::bindable_t{
                            "notanint", sqlw::Type::SQL_INT}});

                ([&]() {
                    ASSERT_EQ(sqlw::status::Code::UNUSED_PARAMETERS_ERROR, ec);
                    ASSERT_STREQ("", ss.str().data());
                })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"},
        // Passing sql string + tuple of parameters to bind + callback. Multiple
        // statements. Fatal error in third statement.
        test_case_t{
            "H6",
            [](sqlw::Transaction& t) {
                std::stringstream ss;
                auto ec = t(
                    "INSERT INTO user (id, name) VALUES (3,?1),(?2,'Ned');"
                    "UPDATE user SET name='Neddy' WHERE id=6666 RETURNING *;"
                    "UPDATE user SET name='Horus WHERE id=6666 RETURNING *",
                    [&](sqlw::Statement::ExecArgs e) {
                        ss << e.column_value << ",";
                    },
                    std::tuple{
                        sqlw::Statement::bindable_t{
                            "Bob", sqlw::Type::SQL_TEXT},
                        6,
                    });

                ([&]() { ASSERT_STREQ("", ss.str().data()); })();

                return ec;
            },
            sqlw::status::Condition::ERROR,
            "1,kate,2,eris,"}),
    [](const testing::TestParamInfo<TransactionTest::ParamType>& info) {
        return std::string{std::get<0>(info.param)};
    });
