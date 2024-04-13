#include "sqlw/utils.hpp"
#include <algorithm>

bool sqlw::utils::is_numeric(std::string_view value) noexcept
{
	const auto it = std::find_if(
	    value.begin(),
	    value.end(),
	    [](char const& c)
	    {
		    return !std::isdigit(c) && c != '.';
	    }
	);

	return it == value.end();
}

std::errc sqlw::utils::to_double(std::string_view str, double& result) noexcept
{
	double mod = 1;

	if (
		str.size() == 0
	)
	{
		return std::errc::invalid_argument;
	}
	else if ('-' == str[0])
	{
		mod = -1;
		str = str.substr(1);
	}
	else if (!std::isdigit(str[0]))
	{
		return std::errc::invalid_argument;
	}

	size_t dot = 0;

	// validation
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '.')
		{
			if (dot != 0)
			{
				return std::errc::invalid_argument;
			}

			dot = i;
		}
		else if (!std::isdigit(str[i])) {
			return std::errc::invalid_argument;
		}
	}

	// digits10 is number of digits that can be represented without
	// loss or mangling.
	constexpr size_t digits10 = static_cast<size_t>(std::numeric_limits<double>::digits10);

	const int whole_end = dot - 1;

	size_t exp = 1;
	for (int i = whole_end; i >= 0; i--)
	{
		int n = str[i] - '0';
		result += n * exp;
		exp *= 10;
	}

	if (dot > digits10)
	{
		dot = str.size();
	}

	exp = 10;
	const size_t part_end = std::min(digits10 + 1, str.size());

	for (size_t i = dot + 1; i < part_end; i++)
	{
		double n = str[i] - '0';
		result += n / exp;
		exp *= 10;
	}

	result = mod * result;

	return std::errc();
}
