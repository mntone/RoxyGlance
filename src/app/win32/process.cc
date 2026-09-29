#include "pch.h"
#include "process.h"

#include "hresult.h"

namespace {
inline constexpr std::wstring_view kApplicationHostFilename = L"\\windows\\system32\\applicationframehost.exe";
inline constexpr std::wstring_view kCoreWindowClass = L"Windows.UI.Core.CoreWindow";
}

static ROXYG_ALWAYS_INLINE constexpr size_t align_down_4x128(size_t x) noexcept {
  if (x < 128) {
    return 128;
  }

  size_t const width = std::bit_width(x);
  size_t const target_shift = (width % 2 == 0) ? (width - 1) : (width - 2);
  return 1uz << target_shift;
}

using namespace roxyg;

static ROXYG_ALWAYS_INLINE DWORD GetProcessImageNameFromHANDLEWithBufferSize(HANDLE hprocess, size_t buffer_size, std::wstring& filepath) {
#if defined(__cpp_lib_string_resize_and_overwrite) && __cpp_lib_string_resize_and_overwrite >= 202110L
  DWORD lasterr = ERROR_SUCCESS;
  filepath.resize_and_overwrite(buffer_size, [&lasterr, hprocess](wchar_t* buf, size_t buflen) noexcept -> size_t {
    DWORD len = static_cast<DWORD>(buflen);
    BOOL const rc = QueryFullProcessImageNameW(hprocess, 0, buf, &len);
    if (rc == FALSE) {
      lasterr = WINRT_IMPL_GetLastError();
      return 0;
    }

    return static_cast<size_t>(len);
  });
  return lasterr;
#else
  filepath.resize(buffer_size);

  DWORD len = static_cast<DWORD>(buffer_size);
  BOOL const rc = QueryFullProcessImageNameW(hprocess, 0, filepath.data(), &len);
  if (rc == FALSE) {
    return WINRT_IMPL_GetLastError();
  }

  filepath.resize(len);
  return ERROR_SUCCESS;
#endif
}

static ROXYG_ALWAYS_INLINE winrt::hresult GetProcessImageNameFromHANDLE(HANDLE hprocess, std::wstring& filepath) noexcept {
  try {
    DWORD lasterr = ERROR_SUCCESS;
    for (
      size_t buffer_size = std::clamp(filepath.capacity(), 128uz, 32768uz);
      buffer_size <= 32768;
      buffer_size = align_down_4x128(buffer_size << 2)
    ) {
      lasterr = GetProcessImageNameFromHANDLEWithBufferSize(hprocess, buffer_size - 1, filepath);
      if (lasterr != ERROR_INSUFFICIENT_BUFFER) {
        break;
      }
    }
    return win32::hresult::HResultFromWin32(lasterr);
  } catch (std::bad_alloc const&) {
    return E_OUTOFMEMORY;
  }
}

static winrt::hresult GetProcessImageNameFromId(DWORD process_id, std::wstring& filepath) noexcept {
  HANDLE process{OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id)};
  if (!process) {
    return win32::hresult::LastErrorAsHResult();
  }

  winrt::hresult const hr = GetProcessImageNameFromHANDLE(process, filepath);
  BOOL const rc = CloseHandle(process);
  if (rc == FALSE && hr == S_OK) {
    return win32::hresult::LastErrorAsHResult();
  }
  return hr;
}

static HWND GetCoreWindowFromParent(HWND parent) {
  if (!parent) {
    return nullptr;
  }

  wchar_t class_name[256];
  HWND child = GetWindow(parent, GW_CHILD);
  while (child != nullptr) {
    if (GetClassNameW(child, class_name, 256) > 0) {
      if (class_name == kCoreWindowClass) {
        return child;
      }
    }
    child = GetWindow(child, GW_HWNDNEXT);
  }
  return nullptr;
}

static ROXYG_ALWAYS_INLINE bool EndsWithOrdinalIgnoreCase(std::wstring_view target, std::wstring_view suffix) noexcept {
  size_t const target_length = target.size();
  size_t const suffix_length = suffix.size();
  if (target_length < suffix_length) {
    return false;
  }

  return CompareStringOrdinal(
    target.data() + target_length - suffix_length,
    static_cast<int>(suffix_length),
    suffix.data(),
    static_cast<int>(suffix_length),
    TRUE
  ) == CSTR_EQUAL;
}

winrt::hresult win32::GetProcessInfoFromHWND(HWND hwnd, ProcessInfo& info) noexcept {
  DWORD process_id;
  DWORD rc = GetWindowThreadProcessId(hwnd, &process_id);
  if (rc == 0) {
    return win32::hresult::LastErrorAsHResult();
  }

  std::wstring filepath;
  winrt::hresult hr = GetProcessImageNameFromId(process_id, filepath);
  if (FAILED(hr)) {
    return hr;
  }

  bool const is_application_host = EndsWithOrdinalIgnoreCase(filepath, kApplicationHostFilename);
  if (!is_application_host) {
    info.id = process_id;
    info.filepath = std::move(filepath);
    return S_OK;
  }

  // FindWindowExW does not find the hosted CoreWindow here, so enumerate child windows instead.
  HWND const child_window_handle = GetCoreWindowFromParent(hwnd);
  if (!child_window_handle) {
    info.id = process_id;
    info.filepath = std::move(filepath);
    return S_OK;
  }

  DWORD real_process_id;
  rc = GetWindowThreadProcessId(child_window_handle, &real_process_id);
  if (rc == 0) {
    return win32::hresult::LastErrorAsHResult();
  }
  if (real_process_id == 0) {
    info.id = process_id;
    info.filepath = std::move(filepath);
    return S_OK;
  }

  hr = GetProcessImageNameFromId(real_process_id, filepath);
  if (FAILED(hr)) {
    return hr;
  }

  info.id = real_process_id;
  info.filepath = std::move(filepath);
  return S_OK;
}
