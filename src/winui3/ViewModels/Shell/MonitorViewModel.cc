#include "pch.h"
#include "MonitorViewModel.h"
#if __has_include("ViewModels/Shell/MonitorViewModel.g.cpp")
#include "ViewModels/Shell/MonitorViewModel.g.cpp"
#endif
#include "ShellResource.h"

#include "../../../app/monitor/MonitorState.h"  // requires monitor::State
#include "../../../app/win32/string.h"          // requires FormatResourceString

namespace winrt {
using namespace ::winrt::Windows::Foundation;
}

using namespace roxyg::win32;

namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation {

static ROXYG_ALWAYS_INLINE hstring makeName(roxyg::monitor::State const& state) {
  using namespace roxyg::winui3::viewmodel::string;

  auto const& friendlyName = state.friendlyName();
  return friendlyName.empty()
    ? FormatResourceString(kShellDisplayFormat, state.id())
    : FormatResourceString(kShellDisplayNameFormat, state.id(), friendlyName);
}

static ROXYG_ALWAYS_INLINE Rect makeRect(roxyg::numeric::float4 vec) noexcept {
  return Rect(vec.x(), vec.y(), vec.z(), vec.w());
}

MonitorViewModel::MonitorViewModel(
  roxyg::monitor::State const& state,
  std::int32_t id, hstring name
)
  : IsPrimary_(state.isPrimary())
  , Id_(id)
  , Name_(name)
  , DisplayArea_(makeRect(state.displayArea()))
  , WorkArea_(makeRect(state.workArea())) {
}

MonitorViewModel::MonitorViewModel(roxyg::monitor::State const& state)
  : MonitorViewModel(state, state.id(), makeName(state)){
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation
