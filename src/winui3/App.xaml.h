#pragma once
#include "App.xaml.g.h"

#include "../app/AppDelegate.h"

namespace winrt::Mntone::RoxyGlance::implementation {

struct App: AppT<App> {
  App();

  void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

  constexpr roxyg::AppDelegate& AppDelegate() noexcept { return AppDelegate_; }

private:
  roxyg::AppDelegate AppDelegate_;
  winrt::Microsoft::UI::Xaml::Window window_;
};

}
