#pragma once

#include "../logging/ILogger.h"
#include "../window/IWindowStateListener.h"

namespace roxyg::debug {

class WindowEventLogAdapter final
  : public window::IWindowStateListener {
  WindowEventLogAdapter(WindowEventLogAdapter const&) = delete;
  WindowEventLogAdapter& operator=(WindowEventLogAdapter const&) = delete;

public:
  constexpr WindowEventLogAdapter() noexcept = default;

  void onForegroundEvent(window::State& state) noexcept override {
    handleEvent(state, L"EVENT_SYSTEM_FOREGROUND");
  }

  void onShowEvent(window::State& state) noexcept override {
    handleEvent(state, L"EVENT_OBJECT_SHOW");
  }

  void setLogger(std::shared_ptr<logging::ILogger> logger) noexcept {
    logger_ = std::move(logger);
  }

private:
  void handleEvent(window::State& state, std::wstring_view eventName) noexcept;

private:
  std::shared_ptr<logging::ILogger> logger_;
};

}
