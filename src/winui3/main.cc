#include "pch.h"
#include "App.xaml.h"

#include <wil/resource.h>

namespace {

inline constexpr std::wstring_view kAppSingleInstanceName = L"P";
inline constexpr DWORD kRedirectActivationTimeout = 20'000;  // 20s

}

namespace winrt {
using namespace ::winrt::Windows::Foundation;
using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::Windows::AppLifecycle;
}

struct RedirectState final {
  wil::unique_event_nothrow hevent;
  std::atomic<HRESULT> hr{S_OK};
};

static winrt::fire_and_forget RedirectActivationAsync(
  std::shared_ptr<RedirectState> state,
  winrt::AppInstance mainInstance,
  winrt::AppActivationArguments args
) {
  wil::event_set_scope_exit ensure_signaled{wil::SetEvent_scope_exit(state->hevent.get())};
  try {
    co_await mainInstance.RedirectActivationToAsync(args);
  } catch (winrt::hresult_error const& error) {
    state->hr.store(error.code(), std::memory_order_release);
  } catch (...) {
    state->hr.store(E_FAIL, std::memory_order_release);
  }
}

static int RedirectActivation(
  winrt::AppInstance mainInstance,
  winrt::AppActivationArguments args,
  DWORD timeout = INFINITE
) {
  std::shared_ptr<RedirectState> state{std::make_shared<RedirectState>()};
  winrt::hresult hr = state->hevent.create();
  if (FAILED(hr)) {
    OutputDebugStringW(L"error: creating event");
    return EXIT_FAILURE;
  }
  RedirectActivationAsync(state, mainInstance, args);

  DWORD handleIndex = 0;
  HANDLE rawHandle = state->hevent.get();
  hr = CoWaitForMultipleObjects(CWMO_DEFAULT, timeout, 1, &rawHandle, &handleIndex);
  if (hr == RPC_S_CALLPENDING) {
    OutputDebugStringW(L"error: timeout on event");
    return EXIT_FAILURE;
  }
  if (FAILED(hr)) {
    OutputDebugStringW(L"error: waiting on event");
    return EXIT_FAILURE;
  }

  hr = state->hr.load(std::memory_order_acquire);
  if (FAILED(hr)) {
    OutputDebugStringW(L"error: activation");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void ApplicationMainInstanceActivated(
  winrt::IInspectable const&,
  [[maybe_unused]] winrt::AppActivationArguments const& args
) {
}

static winrt::Application ApplicationStartCallback([[maybe_unused]] winrt::ApplicationInitializationCallbackParams const& params) {
  return winrt::make<::winrt::Mntone::RoxyGlance::implementation::App>();
}

int __stdcall wWinMain(
  [[maybe_unused]] _In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE,
  [[maybe_unused]] _In_ LPWSTR lpCmdLine,
  [[maybe_unused]] _In_ int nCmdShow
) {
  winrt::init_apartment(winrt::apartment_type::single_threaded);

  winrt::AppInstance const currentInstance{winrt::AppInstance::GetCurrent()};
  winrt::AppInstance const mainInstance{currentInstance.FindOrRegisterForKey(kAppSingleInstanceName)};
  if (!mainInstance.IsCurrent()) {
    winrt::AppActivationArguments const args{currentInstance.GetActivatedEventArgs()};
    int const rc = RedirectActivation(mainInstance, args, kRedirectActivationTimeout);
    return rc;
  }

  winrt::AppInstance::Activated_revoker revoker{
    mainInstance.Activated(winrt::auto_revoke, ApplicationMainInstanceActivated)
  };
  winrt::Application::Start(ApplicationStartCallback);
  return EXIT_SUCCESS;
}
