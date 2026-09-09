#pragma once

namespace roxyg::settings {

enum class StringCompareType: std::uint_fast8_t {
  kContains = 0,
  kStartsWith = (1 << 0),
  kEndsWith = (1 << 1),
  kEquals = kStartsWith | kEndsWith,
  kNone = (1 << 3),
};

using StringAndCompareType = std::pair<StringCompareType, std::wstring>;

}
