#include "pch.h"
#include "SettingsStore.h"

#include <winrt/Windows.Storage.h>

#include "constants.h"
#include "../win32/file.h"
#include "../win32/hresult.h"

using namespace roxyg::settings;

void SettingsStore::reloadUserSettings() {
  winrt::hstring dirPath{winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path()};
  std::filesystem::path filePath{dirPath.begin(), dirPath.end()};
  filePath += kUserSettingsFileName;

  std::string content;
  winrt::hresult hr = win32::ReadFile(filePath, content);
  switch (hr) {
  case S_OK:
    user_settings_.load(content);
    notify(&ISettingsListener::onSettingsChanged, user_settings_);
    break;
  case win32::hresult::kErrorFileNotFound:
  case win32::hresult::kErrorInvalidUserBuffer:
    break;
  case win32::hresult::kErrorNotEnoughMemory:
  case win32::hresult::kErrorAccessDenied:
  case win32::hresult::kErrorSharingViolation:
    break; // TODO: retry
  default:
    winrt::check_hresult(hr);
  }
}
