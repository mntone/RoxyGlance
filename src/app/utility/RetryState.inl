#pragma once
#include "RetryState.h"

namespace roxyg::utility {

template<typename T>
concept BackoffPolicy = requires(T s, T const const_s) {
  { s.reset() } noexcept;
  { const_s.current() } noexcept -> std::integral;
  { s.next() } noexcept -> std::integral;

  requires std::is_nothrow_move_constructible_v<T>;
  requires std::same_as<decltype(const_s.current()), decltype(s.next())>;
};

template<uint32_t MaxAttempts, BackoffPolicy Backoff>
class RetryState final: public IRetryState {
  RetryState(RetryState const&) = delete;
  RetryState& operator=(RetryState const&) = delete;

public:
  static constexpr uint32_t kMaxAttempts{MaxAttempts};

  ROXYG_ALWAYS_INLINE constexpr RetryState(Backoff backoff = {}) noexcept
    : attempts_(0)
    , backoff_(std::move(backoff)) {
  }

  ROXYG_ALWAYS_INLINE constexpr void reset() noexcept {
    attempts_ = 0;
    backoff_.reset();
  }

  constexpr void advanceAttempt() noexcept override {
    ++attempts_;
  }
  ROXYG_ALWAYS_INLINE constexpr uint32_t currentDelay() const noexcept {
    return backoff_.current();
  }
  constexpr void forceExpire() noexcept override {
    attempts_ = MaxAttempts;
  }
  constexpr uint32_t nextDelay() noexcept override {
    return backoff_.next();
  }

  constexpr bool available() const noexcept override {
    return attempts_ < MaxAttempts;
  }
  constexpr bool expired() const noexcept override {
    return attempts_ >= MaxAttempts;
  }
  ROXYG_ALWAYS_INLINE constexpr uint32_t attempts() const noexcept {
    return attempts_;
  }

private:
  uint32_t attempts_;
  Backoff backoff_;
};

template<uint32_t MaxAttempts, BackoffPolicy Backoff>
class RetryStateFactory final: public IRetryStateFactory {
  RetryStateFactory(RetryStateFactory const&) = delete;
  RetryStateFactory& operator=(RetryStateFactory const&) = delete;

public:
  ROXYG_ALWAYS_INLINE constexpr RetryStateFactory(Backoff backoff = {}) noexcept
    : backoff_(std::move(backoff)) {
  }

  constexpr std::unique_ptr<IRetryState> make() const override {
    return std::make_unique<RetryState<MaxAttempts, Backoff>>(backoff_);
  }

private:
  Backoff backoff_;
};

}
