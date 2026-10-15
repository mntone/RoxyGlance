#pragma once
#include "ViewModels/Shell/MonitorViewModel.g.h"

namespace roxyg::monitor {
struct State;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation {

struct MonitorViewModel: MonitorViewModelT<MonitorViewModel> {
  MonitorViewModel(roxyg::monitor::State const& state);
  MonitorViewModel(roxyg::monitor::State const& state, std::int32_t id, hstring name);

  inline constexpr std::int32_t Id() const noexcept {
    return Id_;
  }
  inline hstring Name() const noexcept {
    return Name_;
  }
  inline constexpr bool IsPrimary() const noexcept {
    return IsPrimary_;
  }
  inline constexpr Windows::Foundation::Numerics::float4 DisplayArea() const noexcept {
    return DisplayArea_;
  }
  inline constexpr Windows::Foundation::Numerics::float4 WorkArea() const noexcept {
    return WorkArea_;
  }

private:
  bool IsPrimary_;
  std::int32_t Id_;
  hstring Name_;
  Windows::Foundation::Numerics::float4 DisplayArea_;
  Windows::Foundation::Numerics::float4 WorkArea_;
};

}
