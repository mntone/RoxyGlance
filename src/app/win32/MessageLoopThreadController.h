#pragma once

#include "../logging/LogHelper.h"
#include "../win32/hresult.h"
#include "../utility/RetryState.h"

namespace roxyg::win32 {

enum class ThreadStopFailurePolicy: uint8_t {
  kReturnTimeout,
  kFailFast,
};

class MessageLoopThreadController {
  MessageLoopThreadController(MessageLoopThreadController const&) = delete;
  MessageLoopThreadController& operator=(MessageLoopThreadController const&) = delete;

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

  MessageLoopThreadController() noexcept;
  explicit MessageLoopThreadController(ThreadStopFailurePolicy stop_failure_policy) noexcept;
  MessageLoopThreadController(ThreadStopFailurePolicy stop_failure_policy, std::nullptr_t) = delete;
  explicit MessageLoopThreadController(
    ThreadStopFailurePolicy stop_failure_policy,
    std::unique_ptr<utility::IRetryStateFactory>&& retry_factory
  ) noexcept;
  virtual ~MessageLoopThreadController() noexcept;

  virtual BOOL postStopMessage(intptr_t target) noexcept = 0;

  [[nodiscard]] winrt::hresult start(_beginthreadex_proc_type proc, void* params, ThreadInfo* info) noexcept;
  [[nodiscard]] winrt::hresult stopThread(HANDLE const hthread, intptr_t target) noexcept;
  [[nodiscard]] static winrt::hresult validateThreadAccess(ThreadInfo const& state) noexcept;

private:
  winrt::hresult reapThread(HANDLE hthread) noexcept;

protected:
  [[nodiscard]] inline ThreadInfo const threadInfo() const noexcept {
    return data_.load(std::memory_order_acquire);
  }
  [[nodiscard]] inline HANDLE hThread() const noexcept {
    return data_.load(std::memory_order_acquire).hthread;
  }
  [[nodiscard]] inline DWORD threadId() const noexcept {
    return data_.load(std::memory_order_acquire).thread_id;
  }

  void setRetryFactory(std::nullptr_t) = delete;
  constexpr void setRetryFactory(std::unique_ptr<utility::IRetryStateFactory>&& retry_factory) noexcept {
    retry_factory_ = std::move(retry_factory);
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

public:
  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
  }

protected:
  std::mutex mutex_;
  std::atomic<ThreadInfo> data_;
  std::unique_ptr<utility::IRetryStateFactory> retry_factory_;
  logging::LogHelper<logging::LogGroup::kWin32> logger_;
  State state_;
  ThreadStopFailurePolicy stop_failure_policy_;
};

}
