#pragma once

#include "../win32/hresult.h"

namespace roxyg::win32 {

enum class ThreadStopFailurePolicy: uint8_t {
  kReturnTimeout,
  kFailFast,
};

class ThreadController {
  ThreadController(ThreadController const&) = delete;
  ThreadController& operator=(ThreadController const&) = delete;

protected:
  enum class State: uint32_t {
    kReady = 1 << 0,
    kRunning = 1 << 1,
    kStopping = 1 << 2,
  };
  struct ThreadInfo final {
    HANDLE hthread;
    DWORD thread_id;
  };

  ThreadController() noexcept;
  explicit ThreadController(ThreadStopFailurePolicy stop_failure_policy) noexcept;
  virtual ~ThreadController() noexcept;

  [[nodiscard]] winrt::hresult start(_beginthreadex_proc_type proc, void* params, ThreadInfo* info) noexcept;
  [[nodiscard]] winrt::hresult reapThread(HANDLE hthread) noexcept;
  [[nodiscard]] static winrt::hresult validateThreadAccess(ThreadInfo const& state) noexcept;

  [[nodiscard]] inline ThreadInfo const threadInfo() const noexcept {
    return data_.load(std::memory_order_acquire);
  }
  [[nodiscard]] inline HANDLE hThread() const noexcept {
    return data_.load(std::memory_order_acquire).hthread;
  }
  [[nodiscard]] inline DWORD threadId() const noexcept {
    return data_.load(std::memory_order_acquire).thread_id;
  }

  [[nodiscard]] constexpr ThreadStopFailurePolicy stopFailurePolicy() const noexcept {
    return stop_failure_policy_;
  }
  constexpr void setStopFailurePolicy(ThreadStopFailurePolicy value) noexcept {
#if _DEBUG
    assert(state_ == State::kReady);
#endif

    stop_failure_policy_ = value;
  }

protected:
  std::mutex mutex_;
  std::atomic<ThreadInfo> data_;
  State state_;
  ThreadStopFailurePolicy stop_failure_policy_;
};

}
