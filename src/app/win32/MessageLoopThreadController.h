#pragma once

#include "../logging/LogHelper.h"
#include "../win32/hresult.h"
#include "../utility/RetryState.h"

namespace roxyg::win32 {

enum class ThreadStopFailurePolicy: int8_t {
  kReturnTimeout,
  kFailFast,
};

class MessageLoopThreadController {
  MessageLoopThreadController(MessageLoopThreadController const&) = delete;
  MessageLoopThreadController& operator=(MessageLoopThreadController const&) = delete;

protected:
  enum class State: int8_t {
    kReady,
    kRunning,
    kStopping,
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

  /// <summary>
  /// Reaps the worker thread after it failed to start, without posting a stop message.
  /// </summary>
  /// <param name="hthread">The handle of the worker thread to reap.</param>
  /// <returns>The worker thread's exit code as an HRESULT, or a failure HRESULT if the reap itself failed.</returns>
  [[nodiscard]] winrt::hresult reapThreadAfterStartFailure(HANDLE hthread) noexcept;

  /// <summary>
  /// Posts a stop message to the worker thread, waits for it to exit, and reaps it.
  /// </summary>
  /// <param name="hthread">The handle of the worker thread to stop.</param>
  /// <param name="target">The postStopMessage() target identifying the worker thread.</param>
  /// <returns>The worker thread's exit code as an HRESULT, or a failure HRESULT if the stop sequence failed.</returns>
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
