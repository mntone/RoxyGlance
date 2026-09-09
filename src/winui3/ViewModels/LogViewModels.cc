#include "pch.h"
#include "LogViewModels.h"
#if __has_include("ViewModels.LogViewModels.g.cpp")
#include "ViewModels.LogViewModels.g.cpp"
#endif

#include <boost/circular_buffer.hpp>
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/static_string.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include "../app/engine/Rule.h"
#include "../app/context/AppContext.h"  // require some boost headers
#include "../App.xaml.h"

namespace winrt {
using namespace ::winrt::Windows::Foundation::Collections;
using namespace ::winrt::Microsoft::UI::Dispatching;
using namespace ::winrt::Microsoft::UI::Xaml;

namespace impl {
using namespace ::winrt::Mntone::RoxyGlance::implementation;
using namespace ::winrt::Mntone::RoxyGlance::ViewModels::implementation;
}
}

winrt::impl::LogsViewModel::LogsViewModel() noexcept
  : Logs_(winrt::single_threaded_observable_vector<ViewModels::LogViewModel>()) {
  auto app = Application::Current().as<impl::App>();

  std::shared_ptr<roxyg::logging::ILogSource> source{ app->AppDelegate().context().logSource()};
  source->addCallback([](void* context, roxyg::logging::Log const& log) {
    DispatcherQueue::GetForCurrentThread().TryEnqueue([context, log] {
      LogsViewModel* viewModel = reinterpret_cast<LogsViewModel*>(context);
      viewModel->Logs_.InsertAt(0, make<impl::LogViewModel>(log));
    });
  }, this);
}
