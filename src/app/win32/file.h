#pragma once
#include <filesystem>

namespace roxyg::win32 {

/// <summary>
/// Reads content from a file.
/// </summary>
/// <returns>S_OK on success, or an HRESULT error code.</returns>
winrt::hresult ReadFile(std::filesystem::path filepath, std::string& content) noexcept;

/// <summary>
/// Writes content to a temporary file and atomically replaces the destination.
/// </summary>
/// <returns>S_OK on success, or an HRESULT error code.</returns>
winrt::hresult WriteFileAtomic(std::filesystem::path filepath, std::string_view content) noexcept;

}
