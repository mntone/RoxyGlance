#include "pch.h"
#include "AppDelegate.h"
#include "context/AppContext.h"
#include "engine/Rule.h"

using namespace roxyg;

AppDelegate::AppDelegate() noexcept
  : context_(std::make_unique<AppContext>()) {
}

AppDelegate::~AppDelegate() noexcept = default;
