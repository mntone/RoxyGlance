#pragma once
#include "String.h"

namespace roxyg::settings {

class Filter final {
public:
  explicit Filter(c4::yml::NodeRef node);

  [[nodiscard]] constexpr StringAndMatchType const& processImageName() const noexcept {
    return process_image_name_;
  }
  void setProcessImageName(StringAndMatchType val);

  [[nodiscard]] constexpr StringAndMatchType const& windowClass() const noexcept {
    return window_class_;
  }
  void setWindowClass(StringAndMatchType val);

  [[nodiscard]] constexpr StringAndMatchType const& windowTitle() const noexcept {
    return window_title_;
  }
  void setWindowTitle(StringAndMatchType val);

private:
  c4::yml::NodeRef node_;
  StringAndMatchType process_image_name_;
  StringAndMatchType window_class_;
  StringAndMatchType window_title_;
};

}
