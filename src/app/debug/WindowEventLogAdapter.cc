#include "pch.h"
#include "WindowEventLogAdapter.h"

#include "../window/WindowState.h"

using namespace roxyg::debug;

void WindowEventLogAdapter::handleEvent(window::State& state, std::wstring_view eventName) noexcept {
  winrt::hstring log = winrt::format(
    L"{} (hwnd={:#010x}, pid={}, class={}): {}",
    state.windowTitle(),
    reinterpret_cast<intptr_t>(state.hWnd()),
    state.processId(),
    state.windowClass(),
    eventName);
  logger_.trace(std::move(log));
}
