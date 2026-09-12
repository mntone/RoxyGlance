#pragma once

namespace roxyg::win32 {

class ThreadController {
  ThreadController(ThreadController const&) = delete;
  ThreadController& operator=(ThreadController const&) = delete;

protected:
  struct ThreadState final {
    HANDLE hthread;
    DWORD thread_id;
  };

  ThreadController() noexcept;
#if _DEBUG
  ~ThreadController() noexcept;
#endif

  [[nodiscard]] winrt::hresult start(_beginthreadex_proc_type proc, void* params) noexcept;
  [[nodiscard]] DWORD validateThreadAccess(ThreadState const& state) noexcept;
  [[nodiscard]] DWORD reapThread(HANDLE hthread) noexcept;

  [[nodiscard]] inline ThreadState const threadState() const noexcept {
    return state_.load(std::memory_order_acquire);
  }
  [[nodiscard]] inline HANDLE hThread() const noexcept {
    return state_.load(std::memory_order_acquire).hthread;
  }
  [[nodiscard]] inline DWORD threadId() const noexcept {
    return state_.load(std::memory_order_acquire).thread_id;
  }

protected:
  std::mutex mutex_;
  std::atomic<ThreadState> state_;
};

}
