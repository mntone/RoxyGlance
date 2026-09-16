#include "pch.h"
#include "WindowStateCache.h"

using namespace roxyg::window;

void StateCache::processWindowEvent(DWORD event, HWND hwnd) noexcept {
  [[assume(event != EVENT_SYSTEM_FOREGROUND && event != EVENT_OBJECT_SHOW)]];

  switch (event) {
  case EVENT_OBJECT_DESTROY:
    remove(hwnd);
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
