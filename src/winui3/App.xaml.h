#pragma once
#include "App.xaml.g.h"

#include "../app/AppDelegate.h"
#include "Views/MainWindow.xaml.h"

namespace winrt::Mntone::RoxyGlance::implementation {

struct App: AppT<App> {
  App();

  void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

  constexpr roxyg::AppDelegate& AppDelegate() noexcept { return AppDelegate_; }

private:
  void showMainWindow();

private:
  roxyg::AppDelegate AppDelegate_;
  winrt::com_ptr<winrt::Mntone::RoxyGlance::Views::implementation::MainWindow> MainWindow_;
};

}
