#include "pch.h"
#include "SettingsStore.h"

#include <winrt/Windows.Storage.h>

#include "constants.h"
#include "../win32/file.h"

using namespace roxyg::settings;

void SettingsStore::reloadUserSettings() {
  winrt::hstring dirPath{winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path()};
  std::filesystem::path filePath{dirPath.begin(), dirPath.end()};
  filePath += kUserSettingsFileName;

  std::string content;
  winrt::hresult hr = win32::ReadFile(filePath, content);
  switch (hr) {
  case win32::hresult::kOk:
    user_settings_.load(content);
    notify(&ISettingsListener::onSettingsChanged, user_settings_);
    break;
  case win32::hresult::kFileNotFound:
  case win32::hresult::kPathNotFound:
  case win32::hresult::kInvalidUserBuffer:
    break;
  case win32::hresult::kTooManyOpenFiles:
  case win32::hresult::kAccessDenied:
  case win32::hresult::kSharingViolation:
    break; // TODO: retry
  default:
    winrt::check_hresult(hr);
  }
}
