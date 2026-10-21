#pragma once

namespace roxyg::message {

struct ROXYG_NOVTABLE IMessageListener {
  virtual void onAppExitRequested() noexcept {}
  virtual void onDisplayChanged() noexcept {}
  virtual void onShowSettingsRequested() noexcept {}

protected:
  ~IMessageListener() = default;
};

}
