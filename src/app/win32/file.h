#pragma once
#include <filesystem>

namespace roxyg::win32 {

winrt::hresult ReadFile(std::filesystem::path filepath, std::string& content) noexcept;

}
