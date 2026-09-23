#include "pch.h"

#include <windows.h>

#include "app/utility/RetryState.inl"
#include "app/utility/backoff/fixed.h"
#include "app/win32/MessageLoopThreadController.h"
#include "app/win32/hresult.h"

namespace test::roxyg::win32 {

namespace {

using RetryFactory = ::roxyg::utility::RetryStateFactory<
  2,
  ::roxyg::utility::uint32_fixed_backoff<100>
>;

enum class PostResult {
  kSuccess,
  kNotEnoughQuota,
  kFailure,
};

enum class WorkerBehavior {
  kExitOnStop,
  kIgnoreStop,
  kStopSelf,
};

class TestMessageLoopThreadController;

struct WorkerState final {
  HANDLE stop_event;
  HANDLE release_event;
  HANDLE start_gate_event;
  HANDLE ready_event;
  HANDLE stop_complete_event;
  DWORD exit_code;
  WorkerBehavior behavior;
  TestMessageLoopThreadController* controller;
  winrt::hresult self_stop_result;
};

class TestMessageLoopThreadController final: public ::roxyg::win32::MessageLoopThreadController {
public:
  TestMessageLoopThreadController() noexcept
    : MessageLoopThreadController(
      ::roxyg::win32::ThreadStopFailurePolicy::kReturnTimeout,
      std::make_unique<RetryFactory>()
    ) {
  }

  [[nodiscard]] winrt::hresult start(WorkerState* state) noexcept {
    if (hThread() != INVALID_HANDLE_VALUE) {
      return MessageLoopThreadController::start(worker, state, nullptr);
    }

    ResetEvent(state->stop_event);
    ResetEvent(state->release_event);
    ResetEvent(state->start_gate_event);
    ResetEvent(state->ready_event);
    ResetEvent(state->stop_complete_event);
    worker_state_ = state;
    state->controller = this;
    winrt::hresult const hr = MessageLoopThreadController::start(worker, state, nullptr);
    if (SUCCEEDED(hr)) {
      SetEvent(state->start_gate_event);
    }
    return hr;
  }

  [[nodiscard]] winrt::hresult stopThread() noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ != State::kRunning && state_ != State::kStopping) {
      return ::roxyg::win32::hresult::kErrorInvalidOperation;
    }

    ThreadInfo const current_info{threadInfo()};
    winrt::hresult const hr = validateThreadAccess(current_info);
    if (FAILED(hr)) {
      return hr;
    }

    state_ = State::kStopping;
    return MessageLoopThreadController::stopThread(current_info.hthread, 0);
  }

  void setPostResults(std::initializer_list<PostResult> results) {
    post_results_.assign(results);
    post_result_index_ = 0;
  }

  [[nodiscard]] uint32_t postCount() const noexcept {
    return post_count_;
  }

  [[nodiscard]] bool hasThread() const noexcept {
    return hThread() != INVALID_HANDLE_VALUE;
  }

protected:
  BOOL postStopMessage(intptr_t) noexcept override {
    ++post_count_;
    PostResult const result = post_result_index_ < post_results_.size()
      ? post_results_[post_result_index_++]
      : PostResult::kSuccess;
    if (result == PostResult::kSuccess) {
      return SetEvent(worker_state_->stop_event);
    }

    SetLastError(result == PostResult::kNotEnoughQuota ? ERROR_NOT_ENOUGH_QUOTA : ERROR_ACCESS_DENIED);
    return FALSE;
  }

private:
  static unsigned __stdcall worker(void* parameter) noexcept;

  boost::container::small_vector<PostResult, 4> post_results_;
  size_t post_result_index_{};
  uint32_t post_count_{};
  WorkerState* worker_state_{};
};

class MessageLoopThreadControllerTest: public ::testing::Test {
protected:
  void SetUp() override {
    state_.stop_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ASSERT_NE(state_.stop_event, nullptr);
    state_.release_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ASSERT_NE(state_.release_event, nullptr);
    state_.start_gate_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ASSERT_NE(state_.start_gate_event, nullptr);
    state_.ready_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ASSERT_NE(state_.ready_event, nullptr);
    state_.stop_complete_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ASSERT_NE(state_.stop_complete_event, nullptr);
  }

  void TearDown() override {
    SetEvent(state_.release_event);
    if (controller_.hasThread()) {
      controller_.setPostResults({PostResult::kSuccess});
      EXPECT_EQ(controller_.stopThread(), S_OK);
    }
    CloseHandle(state_.stop_complete_event);
    CloseHandle(state_.ready_event);
    CloseHandle(state_.start_gate_event);
    CloseHandle(state_.release_event);
    CloseHandle(state_.stop_event);
  }

  void waitForWorkerReady() {
    ASSERT_EQ(WaitForSingleObject(state_.ready_event, 1000), WAIT_OBJECT_0);
  }

  WorkerState state_{
    .stop_event = nullptr,
    .release_event = nullptr,
    .start_gate_event = nullptr,
    .ready_event = nullptr,
    .stop_complete_event = nullptr,
    .exit_code = 0,
    .behavior = WorkerBehavior::kExitOnStop,
    .controller = nullptr,
    .self_stop_result = S_OK,
  };
  TestMessageLoopThreadController controller_;
};

unsigned __stdcall TestMessageLoopThreadController::worker(void* parameter) noexcept {
  auto* const state = static_cast<WorkerState*>(parameter);
  WaitForSingleObject(state->start_gate_event, INFINITE);
  SetEvent(state->ready_event);

  switch (state->behavior) {
  case WorkerBehavior::kExitOnStop:
    WaitForSingleObject(state->stop_event, INFINITE);
    break;
  case WorkerBehavior::kIgnoreStop:
    WaitForSingleObject(state->release_event, INFINITE);
    break;
  case WorkerBehavior::kStopSelf:
    state->self_stop_result = state->controller->stopThread();
    SetEvent(state->stop_complete_event);
    WaitForSingleObject(state->release_event, INFINITE);
    break;
  }

  return state->exit_code;
}

}

TEST_F(MessageLoopThreadControllerTest, StopsNormallyAfterSendingStopMessage) {
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();

  EXPECT_EQ(controller_.stopThread(), S_OK);
  EXPECT_EQ(controller_.postCount(), 1u);
}

TEST_F(MessageLoopThreadControllerTest, ReturnsThreadExitCodeAfterStopping) {
  state_.exit_code = 42;
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();

  EXPECT_EQ(controller_.stopThread(), static_cast<HRESULT>(state_.exit_code));
}

TEST_F(MessageLoopThreadControllerTest, RetriesAfterNotEnoughQuota) {
  controller_.setPostResults({PostResult::kNotEnoughQuota, PostResult::kSuccess});
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();

  EXPECT_EQ(controller_.stopThread(), S_OK);
  EXPECT_EQ(controller_.postCount(), 2u);
}

TEST_F(MessageLoopThreadControllerTest, ReturnsFailureWhenPostStopMessageFails) {
  controller_.setPostResults({PostResult::kFailure});
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();

  EXPECT_EQ(
    controller_.stopThread(),
    ::roxyg::win32::hresult::HResultFromWin32(ERROR_ACCESS_DENIED)
  );

  controller_.setPostResults({PostResult::kSuccess});
  EXPECT_EQ(controller_.stopThread(), S_OK);
}

TEST_F(MessageLoopThreadControllerTest, CanStopAgainAfterStopMessageFailure) {
  controller_.setPostResults({PostResult::kFailure});
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();
  ASSERT_EQ(
    controller_.stopThread(),
    ::roxyg::win32::hresult::HResultFromWin32(ERROR_ACCESS_DENIED)
  );

  controller_.setPostResults({PostResult::kSuccess});

  EXPECT_EQ(controller_.stopThread(), S_OK);
  EXPECT_EQ(controller_.postCount(), 2u);
}

TEST_F(MessageLoopThreadControllerTest, ReturnsQuotaFailureAfterRetryLimit) {
  controller_.setPostResults({PostResult::kNotEnoughQuota, PostResult::kNotEnoughQuota});
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();

  EXPECT_EQ(controller_.stopThread(), ::roxyg::win32::hresult::kErrorNotEnoughQuota);
  EXPECT_EQ(controller_.postCount(), 2u);
  EXPECT_TRUE(controller_.hasThread());

  controller_.setPostResults({PostResult::kSuccess});
  EXPECT_EQ(controller_.stopThread(), S_OK);
}

TEST_F(MessageLoopThreadControllerTest, ReturnsTimeoutWhenWorkerDoesNotExit) {
  state_.behavior = WorkerBehavior::kIgnoreStop;
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();

  EXPECT_EQ(controller_.stopThread(), ::roxyg::win32::hresult::kErrorTimeout);
  EXPECT_TRUE(controller_.hasThread());

  SetEvent(state_.release_event);
  EXPECT_EQ(controller_.stopThread(), S_OK);
}

TEST_F(MessageLoopThreadControllerTest, CanStartAgainAfterStopping) {
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();
  ASSERT_EQ(controller_.stopThread(), S_OK);

  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();
  EXPECT_EQ(controller_.stopThread(), S_OK);
}

TEST_F(MessageLoopThreadControllerTest, RejectsStopWhenNotStarted) {
  EXPECT_EQ(controller_.stopThread(), ::roxyg::win32::hresult::kErrorInvalidOperation);
}

TEST_F(MessageLoopThreadControllerTest, RejectsStartWhileRunning) {
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();

  EXPECT_EQ(controller_.start(&state_), ::roxyg::win32::hresult::kErrorInvalidOperation);
}

TEST_F(MessageLoopThreadControllerTest, RejectsStopFromWorkerThread) {
  state_.behavior = WorkerBehavior::kStopSelf;
  ASSERT_EQ(controller_.start(&state_), S_OK);
  waitForWorkerReady();
  ASSERT_EQ(WaitForSingleObject(state_.stop_complete_event, 1000), WAIT_OBJECT_0);

  EXPECT_EQ(
    state_.self_stop_result,
    ::roxyg::win32::hresult::HResultFromWin32(ERROR_POSSIBLE_DEADLOCK)
  );

  SetEvent(state_.release_event);
  EXPECT_EQ(controller_.stopThread(), S_OK);
}

}  // namespace test::roxyg::win32
