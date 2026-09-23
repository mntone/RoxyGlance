#pragma once

namespace roxyg::utility {

class IRetryState {
public:
  virtual ~IRetryState() noexcept = default;
  virtual void advanceAttempt() noexcept = 0;
  [[nodiscard]] virtual bool available() const noexcept = 0;
  [[nodiscard]] virtual bool expired() const noexcept = 0;
  virtual void forceExpire() noexcept = 0;
  [[nodiscard]] virtual uint32_t nextDelay() noexcept = 0;
};

struct IRetryStateFactory {
  virtual ~IRetryStateFactory() noexcept = default;
  [[nodiscard]] virtual std::unique_ptr<IRetryState> make() const = 0;
};

}
