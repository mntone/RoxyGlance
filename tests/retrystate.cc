#include "pch.h"
#include "app/utility/backoff/exponential.h"
#include "app/utility/RetryState.h"
#include "app/utility/RetryState.inl"

namespace test::roxyg::utility {

using Backoff = ::roxyg::utility::uint32_exponential_backoff<100, 800, 2.f>;
using RetryState = ::roxyg::utility::RetryState<3, Backoff>;

TEST(RetryState, StartsAvailableWithInitialBackoff) {
  RetryState retry;

  EXPECT_EQ(retry.attempts(), 0u);
  EXPECT_TRUE(retry.available());
  EXPECT_FALSE(retry.expired());
  EXPECT_EQ(retry.currentDelay(), 100u);
  EXPECT_EQ(retry.nextDelay(), 200u);
}

TEST(RetryState, AdvancesBackoffAndAttemptsSeparately) {
  RetryState retry;

  EXPECT_EQ(retry.nextDelay(), 200u);
  EXPECT_EQ(retry.currentDelay(), 200u);
  EXPECT_EQ(retry.attempts(), 0u);

  retry.advanceAttempt();

  EXPECT_EQ(retry.attempts(), 1u);
  EXPECT_TRUE(retry.available());
  EXPECT_FALSE(retry.expired());
  EXPECT_EQ(retry.nextDelay(), 400u);
}

TEST(RetryState, ExpiresAtMaximumAttempts) {
  RetryState retry;

  retry.advanceAttempt();
  retry.advanceAttempt();
  EXPECT_TRUE(retry.available());
  EXPECT_FALSE(retry.expired());

  retry.advanceAttempt();

  EXPECT_EQ(retry.attempts(), 3u);
  EXPECT_FALSE(retry.available());
  EXPECT_TRUE(retry.expired());
}

TEST(RetryState, ForceExpireMakesStateUnavailable) {
  RetryState retry;

  retry.forceExpire();

  EXPECT_FALSE(retry.available());
  EXPECT_TRUE(retry.expired());
  EXPECT_EQ(retry.attempts(), RetryState::kMaxAttempts);
}

TEST(RetryState, ResetRestoresRetryAndBackoffState) {
  RetryState retry;

  retry.nextDelay();
  retry.advanceAttempt();
  retry.advanceAttempt();
  retry.reset();

  EXPECT_EQ(retry.attempts(), 0u);
  EXPECT_TRUE(retry.available());
  EXPECT_FALSE(retry.expired());
  EXPECT_EQ(retry.currentDelay(), 100u);
  EXPECT_EQ(retry.nextDelay(), 200u);
}

TEST(RetryStateFactory, CreatesRetryStateThroughInterface) {
  ::roxyg::utility::RetryStateFactory<3, Backoff> factory;
  std::unique_ptr<::roxyg::utility::IRetryState> retry{factory.make()};

  EXPECT_TRUE(retry->available());
  EXPECT_FALSE(retry->expired());
  EXPECT_EQ(retry->nextDelay(), 200u);
}

}  // namespace test::roxyg::utility
