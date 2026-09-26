#pragma once

namespace roxyg::message {

struct IMessageListener {
  virtual void onAppExitRequested() noexcept = 0;
  virtual void onShowSettingsRequested() noexcept = 0;

protected:
  ~IMessageListener() = default;
};

}
