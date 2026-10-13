#include "pch.h"
#include "NotifyPropertyChangedHolder.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Microsoft::UI::Xaml::Data;
}

using namespace roxyg::winui3::viewmodel;

winrt::event_token detail::NotifyPropertyChangedHolderBase::PropertyChanged(
  winrt::PropertyChangedEventHandler const& handler
) {
  return property_changed_.add(handler);
}

void detail::NotifyPropertyChangedHolderBase::PropertyChanged(winrt::event_token const token) noexcept {
  property_changed_.remove(token);
}
