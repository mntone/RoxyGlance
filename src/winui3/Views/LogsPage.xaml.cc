#include "pch.h"
#include "LogsPage.xaml.h"
#if __has_include("Views/LogsPage.g.cpp")
#include "Views/LogsPage.g.cpp"
#endif

#include "../AppLocator.h"
#include "../ViewModels/Logging/LogsViewModel.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation;

using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Logging;
}

using namespace roxyg::winui3;

namespace winrt::Mntone::RoxyGlance::Views::implementation {
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::Logging::implementation;

LogsPage::LogsPage(roxyg::logging::Logger& logger)
  : Logs_(make<LogsViewModel>(logger)) {
}

LogsPage::LogsPage()
  : LogsPage(GetAppDelegate().logger()) {
}

}
