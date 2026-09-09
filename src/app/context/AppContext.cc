#include "pch.h"
#include "AppContext.h"

#include "../engine/Rule.h"
#include "../window/WindowStateWatcher.h"

using namespace roxyg;

AppContext::AppContext() noexcept {
  engine_.setOperationContext(&operation_context_);
  settings_store_.addListener(&engine_);
  settings_store_.reloadUserSettings();
}

winrt::hresult AppContext::initialize(
    std::shared_ptr<logging::ILogger> logger,
    std::shared_ptr<logging::ILogSource> logSource
) noexcept {
  log_source_ = std::move(logSource);

  // memo
  // 1. AppContext シャットダウン処理を行おうとするとき、
  // 2. まず RuleEngine を StateWatcher から購読解除する
  window::StateWatcher& w = window::StateWatcher::instance();
  w.addListener(&engine_);

#if _DEBUG
  log_adapter_.setLogger(logger);
  w.addListener(&log_adapter_);
#endif

  return w.start();
}
