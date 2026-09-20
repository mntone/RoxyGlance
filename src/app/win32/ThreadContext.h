#pragma once

#include "../win32/hresult.h"

namespace roxyg::win32 {

class ThreadContext {
public:
  __forceinline constexpr ThreadContext() noexcept
    : hr_(E_PENDING) {
  }

  __forceinline void notify(winrt::hresult hr) noexcept {
    hr_.store(hr, std::memory_order_release);
    hr_.notify_one();
  }

  __forceinline unsigned int notifyLastError() noexcept {
    winrt::hresult const hr{hresult::LastErrorAsHResult()};
    notify(hr);
    _endthreadex(EXIT_FAILURE);
    return 0;
  }

  __forceinline void wait() noexcept {
    hr_.wait(E_PENDING, std::memory_order_acquire);
  }

  __forceinline winrt::hresult wait_and_load() noexcept {
    wait();
    return hr_.load(std::memory_order_acquire);
  }

private:
  std::atomic<HRESULT> hr_;
};

}
