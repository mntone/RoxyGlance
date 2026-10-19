#pragma once
#include "BasicSettingsReader.h"

namespace roxyg::settings {

/// <summary>
/// A settings document that can also be serialized back to text.
/// </summary>
template <typename T>
concept WritableSettingsDocument
  = SettingsDocument<T>
  && requires(T const& doc) {
    { doc.str() } -> std::convertible_to<std::string>;
  };

/// <summary>
/// A BasicSettingsReader that can also save the document and publish edits.
/// </summary>
template<WritableSettingsDocument TDocument>
class BasicSettingsStore: public BasicSettingsReader<TDocument> {
public:
  BasicSettingsStore() noexcept = default;

  /// <summary>
  /// Writes the current document to its file.
  /// </summary>
  /// <returns>S_OK on success, or an HRESULT error code.</returns>
  winrt::hresult save() noexcept {
    try {
      std::string const content{this->document().str()};
      return win32::WriteFileAtomic(detail::ResolveSettingsPath(TDocument::fileName()), content);
    } catch (...) {
      return winrt::to_hresult();
    }
  }

  /// <summary>
  /// Notifies listeners and then saves the document.
  /// </summary>
  /// <returns>S_OK on success, or the error from save().</returns>
  winrt::hresult commit() noexcept {
    this->notify(&ISettingsDocumentListener<TDocument>::onSettingsChanged, this->document());
    return save();
  }
};

}
