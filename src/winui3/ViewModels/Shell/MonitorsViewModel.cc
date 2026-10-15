#include "pch.h"
#include "MonitorsViewModel.h"
#if __has_include("ViewModels/Shell/MonitorsViewModel.g.cpp")
#include "ViewModels/Shell/MonitorsViewModel.g.cpp"
#endif
#include "ShellResource.h"

#include "../../../app/monitor/MonitorStateCache.h"  // requires monitor::StateCache
#include "../../../app/win32/string.h"               // requires LoadResourceString, FormatResourceString

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::Foundation::Collections;

using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Shell;
}

using namespace roxyg::monitor;
using namespace roxyg::win32;

namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation {

template<typename T, typename Allocator = std::allocator<T>>
IVectorView<T> single_threaded_vector_view(std::vector<T, Allocator>&& values = {}) {
  return make<impl::input_vector_view<T, std::vector<T, Allocator>>>(std::move(values));
}

static ROXYG_ALWAYS_INLINE IVectorView<IInspectable>
makeMonitorViewModels(StateCache& monitor) {
  using namespace roxyg::winui3::viewmodel::string;

  StateCache::ContainerType const& storage{monitor.storage()};
  size_t const storage_size{storage.size()};
  if (storage_size == 0) {
    return nullptr;
  }

  std::vector<IInspectable> winrt_monitors(1);
  winrt_monitors.reserve(1 + storage_size);

  State const* primary_state{nullptr};
  for (auto const& [_, state] : storage) {
    winrt_monitors.push_back(make<MonitorViewModel>(state));

    if (state.isPrimary()) {
      primary_state = &state;
    }
  }
  if (!primary_state) {
    throw hresult_invalid_argument();
  }

  auto const& friendlyName = primary_state->friendlyName();
  hstring const name{friendlyName.empty()
    ? LoadResourceString(kShellMain)
    : FormatResourceString(kShellMainFormat, std::wstring_view{friendlyName.c_str(), friendlyName.size()})
  };
  winrt_monitors[0] = make<MonitorViewModel>(*primary_state, 0, name);
  return single_threaded_vector_view(std::move(winrt_monitors));
}

MonitorsViewModel::MonitorsViewModel(StateCache& monitor)
  : Items_(makeMonitorViewModels(monitor)) {
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation
