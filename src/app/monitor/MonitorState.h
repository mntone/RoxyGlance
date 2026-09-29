#pragma once

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
  [[nodiscard]] inline numeric::float4 displayArea() const noexcept { return display_area_; }
  [[nodiscard]] inline numeric::float4 workArea() const noexcept { return work_area_; }

private:
  numeric::float4 display_area_, work_area_;
  HMONITOR const handle_;
  int id_;
};

static_assert(std::is_trivially_copyable<State>::value, "Requires trivially copyable struct");

}
