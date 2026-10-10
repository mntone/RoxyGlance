#pragma once
#include "String.h"

namespace roxyg::settings {

class Filter final {
public:
  explicit Filter(c4::yml::NodeRef node);

  [[nodiscard]] constexpr StringAndMatchType const& processImageName() const noexcept {
    return process_image_name_;
  }
  [[nodiscard]] constexpr StringAndMatchType const& windowClass() const noexcept {
    return window_class_;
  }
  [[nodiscard]] constexpr StringAndMatchType const& windowTitle() const noexcept {
    return window_title_;
  }

private:
  c4::yml::NodeRef node_;
  StringAndMatchType process_image_name_;
  StringAndMatchType window_class_;
  StringAndMatchType window_title_;
};

}
