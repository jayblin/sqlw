#include "sqlw/json_string_result.hpp"
#include "sqlw/forward.hpp"
#include "sqlw/utils.hpp"
#include <algorithm>

static bool is_json_array(const std::string_view& value)
{
	return value[0] == '[' && value[value.length() - 1] == ']';
}

static bool should_be_quoted(const std::string_view& value)
{
	return !(value.length() > 0 && (sqlw::is_numeric(value) || is_json_array(value)));
}

static void close_braces_if_needed(std::stringstream& stream)
{
	if (stream.view().length() > 0 && stream.view().starts_with('{')
	    && !stream.view().ends_with('}'))
	{
		stream << "}";
	}
}

sqlw::JsonStringResult::JsonStringResult(sqlw::JsonStringResult&& other) noexcept
{
	*this = std::move(other);
}

sqlw::JsonStringResult& sqlw::JsonStringResult::operator=(
    sqlw::JsonStringResult&& other
) noexcept
{
	if (this != &other)
	{
		m_stream = std::move(other.m_stream);
	}

	return *this;
}

int sqlw::JsonStringResult::callback(
    void* obj,
    int argc,
    char** argv,
    char** col_name
)
{
	int i;
	const auto stream = &static_cast<JsonStringResult*>(obj)->m_stream;

	if (stream->rdbuf()->in_avail() > 0)
	{
		*stream << "},{";
	}
	else
	{
		*stream << "{";
	}

	for (i = 0; i < argc; i++)
	{
		*stream << '"' << col_name[i] << "\":";

		if (argv[i])
		{
			const auto value = std::string_view {argv[i]};

			if (should_be_quoted(value))
			{
				*stream << '\"' << value << '\"';
			}
			else
			{
				*stream << value;
			}

			if (i + 1 < argc)
			{
				*stream << ',';
			}
		}
		else
		{
			*stream << "null";
		}
	}

	return 0;
}

std::string sqlw::JsonStringResult::get_array_result()
{
	close_braces_if_needed(m_stream);

	m_stream << "]";
	std::stringstream tmp;
	tmp << "[";

	tmp << m_stream.rdbuf();
	m_stream = std::move(tmp);

	return m_stream.str();
}

std::string sqlw::JsonStringResult::get_object_result()
{
	close_braces_if_needed(m_stream);

	return m_stream.str();
}

void sqlw::JsonStringResult::row(int column_count)
{
	if (m_stream.rdbuf()->in_avail() > 0)
	{
		m_stream << "},{";
	}
	else
	{
		m_stream << "{";
	}
}

void sqlw::JsonStringResult::column(
    std::string_view name,
    sqlw::Type type,
    std::string_view value
)
{
	if (!m_stream.view().ends_with('{'))
	{
		m_stream << ',';
	}

	m_stream << '"' << name << "\":";

	if (sqlw::Type::SQL_NULL == type)
	{
		m_stream << "null";
	}
	else if (should_be_quoted(value))
	{
		m_stream << '\"' << value << '\"';
	}
	else
	{
		m_stream << value;
	}
}

bool sqlw::JsonStringResult::has_result()
{
	return m_stream.rdbuf()->in_avail() > 0;
}
