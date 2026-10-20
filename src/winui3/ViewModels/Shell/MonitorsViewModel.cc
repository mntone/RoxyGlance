#include "pch.h"
#include "MonitorsViewModel.h"
#if __has_include("ViewModels/Shell/MonitorsViewModel.g.cpp")
#include "ViewModels/Shell/MonitorsViewModel.g.cpp"
#endif

#include "../../Resources/ShellResource.h"

#include <ranges>     // requires std::range
#include <algorithm>  // requires std::range::sort

#include "../../../app/monitor/MonitorStateCache.h"  // requires monitor::StateCache
#include "../../../app/win32/string.h"               // requires LoadResourceString, FormatResourceString
#include "../../../app/winrt/weak_delegate.h"        // requires weak_delegate

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Windows::Foundation::Collections;

using namespace ::winrt::Microsoft::UI::Dispatching;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Shell;
}

using namespace roxyg::monitor;
using namespace roxyg::win32;
using namespace roxyg::winrt;

namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation {

template<typename T, typename Allocator = std::allocator<T>>
IVectorView<T> single_threaded_vector_view(std::vector<T, Allocator>&& values = {}) {
  return make<impl::input_vector_view<T, std::vector<T, Allocator>>>(std::move(values));
}

static ROXYG_ALWAYS_INLINE IVectorView<IInspectable>
makeMonitorViewModels(StateCache const& monitor) {
  using namespace roxyg::winui3::string;

  StateCache::ContainerType const& storage{monitor.storage()};
  size_t const storage_size{storage.size()};
  if (storage_size == 0) {
    return nullptr;
  }

  boost::container::small_vector<State const*, 5> states_by_id;
  for (auto const& [_, state] : storage) {
    states_by_id.stable_emplace_back(&state);
  }
  std::ranges::sort(states_by_id, {}, &State::id);

  std::vector<IInspectable> winrt_monitors(1);
  winrt_monitors.reserve(1 + storage_size);

  State const* primary_state{nullptr};
  for (State const* state : states_by_id) {
    winrt_monitors.push_back(make<MonitorViewModel>(*state));

    if (state->isPrimary()) {
      primary_state = state;
    }
  }
  if (!primary_state) {
    throw hresult_invalid_argument();
  }

  auto const& friendlyName = primary_state->friendlyName();
  hstring const name{friendlyName.empty()
    ? LoadResourceString(kShellMain)
    : FormatResourceString(kShellMainFormat, friendlyName)
  };
  winrt_monitors[0] = make<MonitorViewModel>(*primary_state, 0, name);
  return single_threaded_vector_view(std::move(winrt_monitors));
}

MonitorsViewModel::MonitorsViewModel(StateCache& monitor)
  : dispatcher_(DispatcherQueue::GetForCurrentThread())
  , monitor_(&monitor)
  , weak_ptr_(get_weak())
  , Items_(makeMonitorViewModels(monitor)) {
  ROXYG_DEBUG_ASSERT_SUCCEEDED(monitor.addCallback(&MonitorsViewModel::onMonitorChanged, get_abi(this)));
}

MonitorsViewModel::~MonitorsViewModel() noexcept {
  monitor_->removeCallback(&MonitorsViewModel::onMonitorChanged, get_abi(this));
}

void MonitorsViewModel::onMonitorChanged(StateCache const& sender, void* context) noexcept {
  MonitorsViewModel* const self{static_cast<MonitorsViewModel*>(context)};
#ifdef _DEBUG
  assert(!self->dispatcher_.HasThreadAccess());
#endif

  auto items = makeMonitorViewModels(sender);
  self->dispatcher_.TryEnqueue(weak_delegate(self->weak_ptr_, [items](com_ptr<MonitorsViewModel> const& viewmodel) {
    viewmodel->Items_ = items;
    viewmodel->notifyPropertyChanged(L"Items");
  }));
}

}  // namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation
