#pragma once
#include "ViewModels/Shell/MonitorsViewModel.g.h"

#include "MonitorViewModel.h"

namespace roxyg::monitor {
class StateCache;
}

namespace winrt::Mntone::RoxyGlance::ViewModels::Shell::implementation {

struct MonitorsViewModel: MonitorsViewModelT<MonitorsViewModel> {
  MonitorsViewModel(roxyg::monitor::StateCache& monitor);

  inline Windows::Foundation::Collections::IVectorView<Windows::Foundation::IInspectable> Items() const noexcept {
    return Items_;
  }

private:
  Windows::Foundation::Collections::IVectorView<Windows::Foundation::IInspectable> Items_;
};

}
