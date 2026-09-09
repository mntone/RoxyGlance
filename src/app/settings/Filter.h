#pragma once
#include "String.h"

namespace roxyg::settings {

class Filter final {
public:
  explicit Filter(c4::yml::NodeRef node);

  [[nodiscard]] constexpr settings::StringAndCompareType const& processName() const noexcept {
    return process_name_;
  }
  [[nodiscard]] constexpr settings::StringAndCompareType const& windowClass() const noexcept {
    return window_class_;
  }
  [[nodiscard]] constexpr settings::StringAndCompareType const& windowTitle() const noexcept {
    return window_title_;
  }

private:
  c4::yml::NodeRef node_;
  settings::StringAndCompareType process_name_;
  settings::StringAndCompareType window_class_;
  settings::StringAndCompareType window_title_;
};

}
