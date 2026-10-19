#pragma once

namespace roxyg::utility {

/// <summary>
/// Tracks retry attempts and the backoff delay between attempts.
/// </summary>
class IRetryState {
public:
  virtual ~IRetryState() noexcept = default;

  /// <summary>
  /// Increments the number of retry attempts.
  /// </summary>
  virtual void advanceAttempt() noexcept = 0;

  /// <summary>
  /// Determines whether another retry attempt is available.
  /// </summary>
  /// <returns><c>true</c> if the retry limit has not been reached; otherwise, <c>false</c>.</returns>
  [[nodiscard]] virtual bool available() const noexcept = 0;

  /// <summary>
  /// Determines whether the retry limit has been reached.
  /// </summary>
  /// <returns><c>true</c> if the retry limit has been reached; otherwise, <c>false</c>.</returns>
  [[nodiscard]] virtual bool expired() const noexcept = 0;

  /// <summary>
  /// Marks the retry state as expired.
  /// </summary>
  virtual void forceExpire() noexcept = 0;

  /// <summary>
  /// Advances the backoff policy and returns the resulting delay.
  /// </summary>
  /// <returns>The delay produced by the backoff policy.</returns>
  [[nodiscard]] virtual uint32_t nextDelay() noexcept = 0;
};

/// <summary>
/// Creates retry states.
/// </summary>
struct IRetryStateFactory {
  virtual ~IRetryStateFactory() noexcept = default;

  /// <summary>
  /// Creates a new retry state.
  /// </summary>
  /// <returns>A newly created retry state.</returns>
  [[nodiscard]] virtual std::unique_ptr<IRetryState> make() const = 0;
};

}
