#include "pch.h"
#include "AppContext.h"

#include "../engine/Rule.h"
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
inline constexpr std::wstring_view kMessageWindowStopAfterInitializationFailure
  = L"Failed to stop the message window after initialization failed.";
inline constexpr std::wstring_view kStateWatcherStopFailed
  = L"Failed to stop the window state watcher.";
inline constexpr std::wstring_view kMessageWindowStopFailed
  = L"Failed to stop the message window.";

}

using namespace roxyg;

AppContext::AppContext() noexcept {
#if _DEBUG
  log_adapter_.setLogger(&logger_);
#endif
  engine_.setOperationContext(&operation_context_);
  engine_.setLogger(&logger_);
  ROXYG_DEBUG_ASSERT_SUCCEEDED(settings_store_.addListener(&engine_));
  message_window_.setLogger(&logger_);
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

  hr = message_window_.start();
  if (FAILED(hr)) {
    context_logger_.error(winrt::hstring{kMessageWindowStartFailed}, hr);
    return hr;
  }

  settings_store_.reloadUserSettings();

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
