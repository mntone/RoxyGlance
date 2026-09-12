#pragma once

namespace roxyg::win32 {

class MessageThreadController final {
  MessageThreadController(MessageThreadController const&) = delete;
  MessageThreadController& operator=(MessageThreadController const&) = delete;

public:
  struct ThreadState final {
    HANDLE hthread;
    unsigned int thread_id;
  };

  MessageThreadController() noexcept;
#if _DEBUG
  ~MessageThreadController() noexcept;
#endif

  [[nodiscard]] winrt::hresult start(_beginthreadex_proc_type proc, void* params) noexcept;
  DWORD stop(DWORD timeout = INFINITE) noexcept;

  [[nodiscard]] inline ThreadState const threadState() const noexcept {
    return state_.load(std::memory_order_acquire);
  }
  [[nodiscard]] inline HANDLE hThread() const noexcept {
    return state_.load(std::memory_order_acquire).hthread;
  }
  [[nodiscard]] inline unsigned int threadId() const noexcept {
    return state_.load(std::memory_order_acquire).thread_id;
  }

private:
  std::mutex mutex_;
  std::atomic<ThreadState> state_;
};

}
