#pragma once

namespace roxyg::settings {

enum class StringMatchType: std::uint_fast8_t {
  kContains = 0,
  kStartsWith = (1 << 0),
  kEndsWith = (1 << 1),
  kEquals = kStartsWith | kEndsWith,
};

using StringAndMatchType = std::pair<StringMatchType, std::wstring>;

}
