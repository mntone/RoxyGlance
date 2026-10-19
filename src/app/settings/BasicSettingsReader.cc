#include "pch.h"
#include "BasicSettingsReader.h"

#include <winrt/Windows.Storage.h>

#include "../win32/file.h"
#include "../win32/hresult.h"

using namespace roxyg::settings;

std::filesystem::path detail::ResolveSettingsPath(std::wstring_view file_name) {
  winrt::hstring const dir_path{winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path()};
  std::filesystem::path file_path{dir_path.begin(), dir_path.end()};
  file_path += file_name;
  return file_path;
}

bool detail::IsTransientReadError(winrt::hresult hr) noexcept {
  return hr == win32::hresult::kErrorAccessDenied
    || hr == win32::hresult::kErrorSharingViolation;
}

bool detail::IsFileMissing(winrt::hresult hr) noexcept {
  return hr == win32::hresult::kErrorFileNotFound;
}
