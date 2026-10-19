#pragma once
#include "ISettingsDocumentListener.h"

#include "../logging/LogHelper.h"
#include "../utility/ListenerHost.h"
#include "../win32/file.h"

namespace roxyg::settings {

/// <summary>
/// A document type that can be stored in a settings file.
/// </summary>
template <typename T>
concept SettingsDocument
  = std::default_initializable<T>
  && requires(T& doc, std::string_view yaml, logging::Logger* logger) {
    { T::fileName() } -> std::convertible_to<std::wstring_view>;
    doc.parse(yaml, logger);
    doc.reset();
  };

namespace detail {

/// <summary>
/// Resolves a file under the application's local folder. May throw.
/// </summary>
/// <param name="file_name">File name starting with a path separator.</param>
std::filesystem::path ResolveSettingsPath(std::wstring_view file_name);

/// <summary>
/// Returns true for failures that may succeed on a later attempt (access denied, sharing violation).
/// </summary>
bool IsTransientReadError(winrt::hresult hr) noexcept;

/// <summary>
/// Returns true if the settings file does not exist.
/// </summary>
bool IsFileMissing(winrt::hresult hr) noexcept;

}  // namespace detail

/// <summary>
/// Owns a settings document, loads it from its file and notifies listeners.
/// </summary>
template<SettingsDocument TDocument>
class BasicSettingsReader
  : public utility::ListenerHost<ISettingsDocumentListener<TDocument>> {
  BasicSettingsReader(BasicSettingsReader const&) = delete;
  BasicSettingsReader& operator=(BasicSettingsReader const&) = delete;

public:
  BasicSettingsReader() noexcept = default;

  /// <summary>
  /// Loads the document from its file and notifies listeners.
  /// A missing or empty file yields an empty document.
  /// </summary>
  /// <returns>S_OK if loaded, or an HRESULT error code (see detail::IsTransientReadError).</returns>
  winrt::hresult load() noexcept {
    try {
      std::string content;
      winrt::hresult const hr = win32::ReadFile(
        detail::ResolveSettingsPath(TDocument::fileName()), content);
      if (FAILED(hr) && !detail::IsFileMissing(hr)) {
        return hr;
      }

      if (content.empty()) {
        document_.reset();
      } else {
        document_.parse(content, logger_.logger());
      }
    } catch (...) {
      return winrt::to_hresult();
    }
    this->notify(&ISettingsDocumentListener<TDocument>::onSettingsChanged, document_);
    return S_OK;
  }

  [[nodiscard]] constexpr TDocument& document() noexcept { return document_; }
  [[nodiscard]] constexpr TDocument const& document() const noexcept { return document_; }

  constexpr void setLogger(logging::Logger* logger) noexcept {
    logger_.setLogger(logger);
  }

private:
  TDocument document_;
  logging::LogHelper<logging::LogGroup::kSettings> logger_;
};

}
