#pragma once
#include <boost/static_string.hpp>

#include "../numeric/numeric.h"

namespace roxyg::monitor {

struct State final {
  friend class StateCache;

  State(std::nullptr_t) = delete;
  explicit State(HMONITOR handle) noexcept;
  explicit State(HMONITOR handle, numeric::float4 display_area) noexcept;

  [[nodiscard]] winrt::hresult initialize() noexcept;

  [[nodiscard]] constexpr int id() const noexcept { return id_; }
  [[nodiscard]] constexpr HMONITOR handle() const noexcept { return handle_; }
  [[nodiscard]] constexpr boost::static_wstring<14> friendlyName() const noexcept { return friendly_name_; }
  [[nodiscard]] constexpr bool isPrimary() const noexcept { return primary_; }
  [[nodiscard]] inline numeric::float4 displayArea() const noexcept { return display_area_; }
  [[nodiscard]] inline numeric::float4 workArea() const noexcept { return work_area_; }

  inline void setFriendlyName(boost::static_wstring<14> const& value) noexcept {
    friendly_name_ = value;
  }

private:
  numeric::float4 display_area_, work_area_;
  boost::static_wstring<14> friendly_name_;
  HMONITOR const handle_;
  int id_;
  bool primary_;
};

static_assert(std::is_trivially_copyable<State>::value, "Requires trivially copyable struct");

}
