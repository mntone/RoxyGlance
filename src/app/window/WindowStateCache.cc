#include "pch.h"
#include "WindowStateCache.h"

using namespace roxyg::window;

State& StateCache::getOrCreate(HWND hwnd) noexcept {
  auto [it, _] = states_.try_emplace(hwnd, hwnd);
  return it->second;
}

void StateCache::processWindowEvent(DWORD event, HWND hwnd) noexcept {
  [[assume(event != EVENT_SYSTEM_FOREGROUND && event != EVENT_OBJECT_SHOW)]];

  switch (event) {
  case EVENT_OBJECT_DESTROY:
    states_.erase(hwnd);
    break;
  case EVENT_OBJECT_LOCATIONCHANGE:
  {
    State* s = get(hwnd);
    if (s) {
      s->invalidateWindowLocation();
    }
    break;
  }
  case EVENT_OBJECT_NAMECHANGE:
  {
    State* s = get(hwnd);
    if (s) {
      s->invalidateWindowTitle();
    }
    break;
  }
  }
}
