#pragma once
#include "ViewModels/Shell/MonitorsViewModel.g.h"

#include "MonitorViewModel.h"

#include "../Holders/NotifyPropertyChangedHolder.h"

namespace roxyg::monitor {
class StateCache;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation {

struct MonitorsViewModel
  : MonitorsViewModelT<MonitorsViewModel>
  , roxyg::winui3::viewmodel::NotifyPropertyChangedHolder<MonitorsViewModel> {
  MonitorsViewModel(roxyg::monitor::StateCache& monitor);
  ~MonitorsViewModel() noexcept;

  inline Windows::Foundation::Collections::IVectorView<Windows::Foundation::IInspectable> Items() const noexcept {
    return Items_;
  }

private:
  static void onMonitorChanged(roxyg::monitor::StateCache const& sender, void* context) noexcept;

private:
  Microsoft::UI::Dispatching::DispatcherQueue const dispatcher_;
  roxyg::monitor::StateCache* const monitor_;
  weak_ref<MonitorsViewModel> const weak_ptr_;
  Windows::Foundation::Collections::IVectorView<Windows::Foundation::IInspectable> Items_;
};

}
