#pragma once

#include "../win32/hresult.h"

namespace roxyg::win32 {

class ThreadContext {
public:
  ROXYG_ALWAYS_INLINE constexpr ThreadContext() noexcept
    : hr_(E_PENDING) {
  }

  ROXYG_ALWAYS_INLINE void notify(winrt::hresult hr) noexcept {
    hr_.store(hr, std::memory_order_release);
    hr_.notify_one();
  }

  ROXYG_ALWAYS_INLINE unsigned int notifyLastError() noexcept {
    winrt::hresult const hr{hresult::LastErrorAsHResult()};
    notify(hr);
    _endthreadex(EXIT_FAILURE);
    return 0;
  }

  ROXYG_ALWAYS_INLINE void wait() noexcept {
    hr_.wait(E_PENDING, std::memory_order_acquire);
  }

  ROXYG_ALWAYS_INLINE winrt::hresult wait_and_load() noexcept {
    wait();
    return hr_.load(std::memory_order_acquire);
  }

private:
  std::atomic<HRESULT> hr_;
};

}
