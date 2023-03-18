#ifndef SQLW_JSON_STRING_RESULT_H_
#define SQLW_JSON_STRING_RESULT_H_

#include "sqlw/concepts.hpp"
#include "sqlw/forward.hpp"
#include <sstream>
#include <string>

namespace sqlw
{
	/**
	 * Object to pass to `sqlite3_exec` function.
	 */
	class JsonStringResult
	{
	public:
		JsonStringResult() {};

		JsonStringResult(const JsonStringResult&) = delete;
		JsonStringResult& operator=(const JsonStringResult&) = delete;

		JsonStringResult(JsonStringResult&&) noexcept;
		JsonStringResult& operator=(JsonStringResult&&) noexcept;

		/**
		 * Callback to pass to `sqlite3_exec` function.
		 */
		static int callback(void* obj, int argc, char** argv, char** col_name);

		auto row(int column_count) -> void;

		auto column(std::string_view name, Type type, std::string_view value)
		    -> void;

		/**
		 * Returns result as json array.
		 */
		auto get_array_result() -> std::string;

		/**
		 * Returns result as json object.
		 */
		auto get_object_result() -> std::string;

		auto has_result() -> bool;

	private:
		std::stringstream m_stream;
	};
} // namespace sqlw

static_assert(sqlw::has_db_callback<sqlw::JsonStringResult>);
static_assert(sqlw::can_be_used_by_statement<sqlw::JsonStringResult>);

#endif // SQLW_JSON_STRING_RESULT_H_
