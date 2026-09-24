#pragma once
#include <filesystem>

namespace roxyg::win32 {

extern winrt::hresult ReadFile(std::filesystem::path filepath, std::string& content) noexcept;

}
