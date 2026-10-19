#include "pch.h"
#include "AppDelegate.h"
#include "context/AppContext.h"
#include "engine/Rule.h"

using namespace roxyg;

AppDelegate::AppDelegate() noexcept
  : context_(std::make_unique<AppContext>()) {
}

AppDelegate::~AppDelegate() noexcept = default;

winrt::hresult AppDelegate::initialize() {
  return context_->initialize();
}

winrt::hresult AppDelegate::exit() noexcept {
  return context_->exit();
}

logging::Logger& AppDelegate::logger() noexcept {
  return context_->logger_;
}

logging::Logger const& AppDelegate::logger() const noexcept {
  return context_->logger_;
}

message::Window& AppDelegate::messageWindow() noexcept {
  return context_->message_window_;
}

message::Window const& AppDelegate::messageWindow() const noexcept {
  return context_->message_window_;
}

monitor::StateCache& AppDelegate::monitorStateCache() noexcept {
  return context_->operation_context_.monitor();
}

monitor::StateCache const& AppDelegate::monitorStateCache() const noexcept {
  return context_->operation_context_.monitor();
}

settings::UserSettingsStore& AppDelegate::settingsStore() noexcept {
  return context_->settings_store_;
}

settings::UserSettingsStore const& AppDelegate::settingsStore() const noexcept {
  return context_->settings_store_;
}
