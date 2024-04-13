#include "sqlw/utils.hpp"
#include <gtest/gtest.h>
#include <limits>

TEST(utils, string_view_to_double)
{
	{
		double r;
		std::errc ec = sqlw::utils::to_double("17.1234567890123", r);

		ASSERT_EQ(std::errc(), ec);
		ASSERT_DOUBLE_EQ(17.1234567890123, r);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("17.12345678901234567", r);

		ASSERT_EQ(std::errc(), ec);
		ASSERT_DOUBLE_EQ(17.1234567890123, r);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("-17.12345678901234567", r);

		ASSERT_EQ(std::errc(), ec);
		ASSERT_DOUBLE_EQ(-17.1234567890123, r);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("994337.001", r);

		ASSERT_EQ(std::errc(), ec);
		ASSERT_DOUBLE_EQ(994337.001, r);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("123456789012345.001", r);

		ASSERT_EQ(std::errc(), ec);
		ASSERT_DOUBLE_EQ(123456789012345.0, r);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("1234567890123456789.001", r);

		ASSERT_EQ(std::errc(), ec);
		ASSERT_DOUBLE_EQ(1234567890123456789.0, r);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("--1234567890123456789.001", r);

		ASSERT_EQ(std::errc::invalid_argument, ec);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("", r);

		ASSERT_EQ(std::errc::invalid_argument, ec);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double(".333", r);

		ASSERT_EQ(std::errc::invalid_argument, ec);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("aaaa", r);

		ASSERT_EQ(std::errc::invalid_argument, ec);
	}

	{
		double r;
		std::errc ec = sqlw::utils::to_double("123.", r);

		ASSERT_EQ(std::errc(), ec);
		ASSERT_DOUBLE_EQ(123.0, r);
	}
}
