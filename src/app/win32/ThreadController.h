#pragma once

#include "../win32/hresult.h"

namespace roxyg::win32 {

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
  virtual ~ThreadController() noexcept;

  [[nodiscard]] winrt::hresult start(_beginthreadex_proc_type proc, void* params) noexcept;
  [[nodiscard]] DWORD validateThreadAccess(ThreadInfo const& state) noexcept;
  [[nodiscard]] winrt::hresult reapThread(HANDLE hthread) noexcept;
  [[nodiscard]] winrt::hresult forceExitThread(HANDLE hthread) noexcept;

  [[nodiscard]] inline ThreadInfo const threadInfo() const noexcept {
    return data_.load(std::memory_order_acquire);
  }
  [[nodiscard]] inline HANDLE hThread() const noexcept {
    return data_.load(std::memory_order_acquire).hthread;
  }
  [[nodiscard]] inline DWORD threadId() const noexcept {
    return data_.load(std::memory_order_acquire).thread_id;
  }

protected:
  std::mutex mutex_;
  std::atomic<ThreadInfo> data_;
  State state_;
};

}
