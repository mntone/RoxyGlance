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
  settings_store_.addListener(&engine_);

  window::StateWatcher& w = window::StateWatcher::instance();
  w.addListener(&engine_);
#if _DEBUG
  w.addListener(&log_adapter_);
#endif

  settings_store_.reloadUserSettings();
}

winrt::hresult AppContext::initialize() {
  return window::StateWatcher::instance().start();
}
