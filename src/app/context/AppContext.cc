#include "pch.h"
#include "AppContext.h"

#include "../engine/Rule.h"
#include "../window/WindowStateWatcher.h"

using namespace roxyg;

AppContext::AppContext() noexcept {
#if _DEBUG
  log_adapter_.setLogger(&logger_);
#endif
  engine_.setOperationContext(&operation_context_);
  engine_.setLogger(&logger_);
  settings_store_.addListener(&engine_);

  window::StateWatcher& w = window::StateWatcher::instance();
  w.setLogger(&logger_);
  w.addListener(&engine_);
#if _DEBUG
  w.addListener(&log_adapter_);
#endif
}

AppContext::~AppContext() noexcept {
  window::StateWatcher::instance().setLogger(nullptr);
}

winrt::hresult AppContext::initialize() {
  winrt::hresult hr = operation_context_.initialize();
  if (FAILED(hr)) {
    return hr;
  }

  settings_store_.reloadUserSettings();
  return window::StateWatcher::instance().start();
}

winrt::hresult AppContext::exit() noexcept {
  winrt::hresult hr = S_OK;

  hr = window::StateWatcher::instance().stop();
  if (FAILED(hr)) {
    // TODO: log
  }

  return hr;
}
