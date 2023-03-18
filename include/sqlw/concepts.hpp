#ifndef SQLW_CONCEPTS_H_
#define SQLW_CONCEPTS_H_

#include "sqlw/forward.hpp"
#include <concepts>
#include <string>

namespace sqlw
{
	// clang-format off
	template<class T>
	concept has_db_callback = requires(
		void* object,
		int argc,
		char** argv,
		char** column_name
	)
	{
		{ T::callback(object, argc, argv, column_name) } -> std::same_as<int>;
	};
	// clang-format on

	// clang-format off
	template<class T>
	concept can_be_used_by_statement = requires(
		T t,
		std::string_view column_name,
		Type column_type,
		std::string_view column_value,
		int column_count
	)
	{
		{ t.row(column_count) } -> std::same_as<void>;
		{ t.column(column_name, column_type, column_value) } -> std::same_as<void>;
	};
	// clang-format on
} // namespace sqlw

#endif // SQLW_CONCEPTS_H_
