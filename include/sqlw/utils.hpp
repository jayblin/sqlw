#ifndef SQL_UTILS_H_
#define SQL_UTILS_H_

#include <string_view>
#include <system_error>

namespace sqlw::utils
{
auto is_numeric(std::string_view) noexcept -> bool;
auto to_double(std::string_view, double&) noexcept -> std::errc;
} // namespace sqlw::utils

#endif // SQL_UTILS_H_
