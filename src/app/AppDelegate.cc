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

bool AppDelegate::addMessageListener(message::IMessageListener* listener) {
  return context_->messageWindow().addListener(listener);
}

bool AppDelegate::removeMessageListener(message::IMessageListener* listener) {
  return context_->messageWindow().removeListener(listener);
}

logging::Logger& AppDelegate::logger() noexcept {
  return context_->logger();
}

logging::Logger const& AppDelegate::logger() const noexcept {
  return context_->logger();
}
