#pragma once

#include "../numeric/numeric.h"

namespace roxyg::monitor {

struct State final {
  friend class StateCache;

  explicit State(HMONITOR hmonitor) noexcept;

  [[nodiscard]] constexpr HMONITOR hMonitor() const noexcept {
    return hmonitor_;
  }

  [[nodiscard]] inline numeric::float4 displayArea() {
    if (area_dirty_) [[unlikely]] {
      updateArea();
    }
    return display_area_;
  }

  [[nodiscard]] inline numeric::float4 workArea() {
    if (area_dirty_) [[unlikely]] {
      updateArea();
    }
    return work_area_;
  }

private:
  void updateArea();

private:
  numeric::float4 display_area_, work_area_;
  HMONITOR hmonitor_;
  boost::static_wstring<CCHDEVICENAME> device_name_;

  bool area_dirty_ : 1;
};

static_assert(std::is_trivially_copyable<State>::value, "Requires trivially copyable struct");

}
