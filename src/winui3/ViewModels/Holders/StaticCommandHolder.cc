#include "pch.h"
#include "StaticCommandHolder.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;
}

using namespace roxyg::winui3::viewmodel;

bool detail::StaticCommandHolderBase::CanExecute(winrt::IInspectable const&) const noexcept {
  return true;
}

winrt::event_token detail::StaticCommandHolderBase::CanExecuteChanged(
  winrt::EventHandler<winrt::IInspectable> const&
) const noexcept {
  return winrt::event_token{0};
}

void detail::StaticCommandHolderBase::CanExecuteChanged(winrt::event_token) const noexcept {
}
