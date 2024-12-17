#ifndef SQLW_CONNECTION_H_
#define SQLW_CONNECTION_H_

#include "sqlite3.h"
#include "sqlw/forward.hpp"
#include <gsl/pointers>
#include <string_view>

namespace sqlw
{
class Connection
{
  public:
    Connection(){};
    Connection(std::string_view filename);

    ~Connection();

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    Connection(Connection&&) noexcept;
    Connection& operator=(Connection&&) noexcept;

    auto connect(std::string_view file_name) -> void;

    auto handle() const -> sqlite3*
    {
        return m_handle;
    }

    auto status() const -> std::error_code
    {
        return m_status;
    }

    auto close() -> void;

  private:
    gsl::owner<sqlite3*> m_handle{nullptr};
    std::error_code m_status{status::Code::CLOSED_HANDLE};
};
} // namespace sqlw

#endif // SQLW_CONNECTION_H_
