#include "pch.h"
#include "AppContext.h"

#include "../engine/Rule.h"
#include "../utility/backoff/exponential.h"
#include "../utility/fastfail.h"
#include "../utility/RetryState.inl"
#include "../win32/hresult.h"
#include "../window/WindowStateWatcher.h"

namespace {

inline constexpr std::wstring_view kOperationContextInitializeFailed
  = L"Failed to initialize the operation context.";
inline constexpr std::wstring_view kMessageWindowInitializeFailed
  = L"Failed to initialize the message window.";
inline constexpr std::wstring_view kMessageWindowStartFailed
  = L"Failed to start the message window.";
inline constexpr std::wstring_view kStateWatcherStartFailed
  = L"Failed to start the window state watcher.";
inline constexpr std::wstring_view kSettingsLoadFailed
  = L"Failed to load the user settings.";
inline constexpr std::wstring_view kMessageWindowStopAfterInitializationFailure
  = L"Failed to stop the message window after initialization failed.";
inline constexpr std::wstring_view kStateWatcherStopFailed
  = L"Failed to stop the window state watcher.";
inline constexpr std::wstring_view kMessageWindowStopFailed
  = L"Failed to stop the message window.";

}

using namespace roxyg;

AppContext::AppContext() noexcept {
  operation_context_.monitor().setLogger(&logger_);
#if _DEBUG
  log_adapter_.setLogger(&logger_);
#endif
  engine_.setOperationContext(&operation_context_);
  engine_.setLogger(&logger_);
  settings_store_.setLogger(&logger_);
  ROXYG_DEBUG_ASSERT_SUCCEEDED(settings_store_.addListener(&engine_));
  message_window_.setLogger(&logger_);
  ROXYG_DEBUG_ASSERT_SUCCEEDED(message_window_.addListener(&operation_context_.monitor()));
  context_logger_.setLogger(&logger_);

  window::StateWatcher& w = window::StateWatcher::instance();
  w.setLogger(&logger_);
  ROXYG_DEBUG_ASSERT_SUCCEEDED(w.addListener(&engine_));
#if _DEBUG
  ROXYG_DEBUG_ASSERT_SUCCEEDED(w.addListener(&log_adapter_));
#endif
}

AppContext::~AppContext() noexcept {
  window::StateWatcher::instance().setLogger(nullptr);
}

winrt::hresult AppContext::initialize() {
  winrt::hresult hr = operation_context_.initialize();
  if (FAILED(hr)) {
    context_logger_.error(winrt::hstring{kOperationContextInitializeFailed}, hr);
    return hr;
  }

  hr = message_window_.initialize();
  if (FAILED(hr)) {
    context_logger_.error(winrt::hstring{kMessageWindowInitializeFailed}, hr);
    return hr;
  }

  utility::RetryState<3, utility::uint32_exponential_backoff<1000, 10000, 2.f>> retry;
  while (true) {
    hr = settings_store_.load();
    if (SUCCEEDED(hr)) {
      break;
    }
    if (!settings::detail::IsTransientReadError(hr)) {
      retry.forceExpire();
      break;
    }

    retry.advanceAttempt();
    if (!retry.available()) {
      break;
    }

    uint32_t const delay{retry.currentDelay()};
    Sleep(delay);
    static_cast<void>(retry.nextDelay());  // Advance the backoff for the next retry.
  }
  if (retry.expired()) {
    if (hr == win32::hresult::kErrorNotEnoughMemory) {
      utility::fastfail();  // continuing without initial settings is meaningless
    }
    context_logger_.error(winrt::hstring{kSettingsLoadFailed}, hr);
  }

  hr = message_window_.start();
  if (FAILED(hr)) {
    context_logger_.error(winrt::hstring{kMessageWindowStartFailed}, hr);
    return hr;
  }

  hr = window::StateWatcher::instance().start();
  if (FAILED(hr)) {
    context_logger_.error(winrt::hstring{kStateWatcherStartFailed}, hr);

    winrt::hresult const msgstop_hr = message_window_.stop();
    if (FAILED(msgstop_hr)) {
      context_logger_.error(
        winrt::hstring{kMessageWindowStopAfterInitializationFailure},
        msgstop_hr
      );
    }
    return hr;
  }

  return S_OK;
}

winrt::hresult AppContext::exit() noexcept {
  winrt::hresult hr = S_OK;

  hr = window::StateWatcher::instance().stop();
  if (FAILED(hr)) {
    context_logger_.error(winrt::hstring{kStateWatcherStopFailed}, hr);
  }

  hr = message_window_.stop();
  if (FAILED(hr)) {
    context_logger_.error(winrt::hstring{kMessageWindowStopFailed}, hr);
  }

  return S_OK;
}
