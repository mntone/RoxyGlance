#include "pch.h"
#include "NodeHelper.h"

#include "constants.h"
#include "../win32/string.h"

namespace magic_enum::customize {

template<>
struct enum_range<roxyg::settings::StringCompareType> {
  static constexpr bool is_flags = true;
};

}

using namespace magic_enum::bitwise_operators;
using namespace roxyg;

long settings::ReadLongFromNode(c4::yml::ConstNodeRef node) {
  if (node.invalid() || !node.has_val()) {
    throw winrt::hresult_invalid_argument(message::kInvalidNodeMessage);
  }

  long val;
  auto check_val = c4::fmt::overflow_checked(val);
  if (!c4::yml::read(node, &check_val)) {
    throw winrt::hresult_invalid_argument(message::kInvalidNumberMessage);
  }
  return val;
}

long settings::ReadBoundedLongFromNode(c4::yml::ConstNodeRef node, long min_val, long max_val) {
  long val = ReadLongFromNode(node);
  if (val < min_val || max_val < val) {
    throw winrt::hresult_invalid_argument(winrt::format(message::kValueOutOfRangeMessage, val));
  }
  return val;
}

float settings::ReadFloatFromNode(c4::yml::ConstNodeRef node) {
  if (node.invalid() || !node.has_val()) {
    throw winrt::hresult_invalid_argument(message::kInvalidNodeMessage);
  }

  float val;
  if (!c4::yml::read(node, &val)) {
    throw winrt::hresult_invalid_argument(message::kInvalidNumberMessage);
  }
  return val;
}

float settings::ReadBoundedFloatFromNode(c4::yml::ConstNodeRef node, float min_val, float max_val) {
  float val = ReadFloatFromNode(node);
  if (val < min_val || max_val < val) {
    throw winrt::hresult_invalid_argument(winrt::format(message::kValueOutOfRangeMessage, val));
  }
  return val;
}

float settings::ReadBoundedFloatFromNodeOrDefault(c4::yml::ConstNodeRef node, float min_val, float max_val, float def_val) {
  if (node.invalid() || !node.has_val()) {
    return def_val;
  }

  float val;
  if (!c4::yml::read(node, &val)) {
    throw winrt::hresult_invalid_argument(message::kInvalidNumberMessage);
  }
  if (val < min_val || max_val < val) {
    throw winrt::hresult_invalid_argument(winrt::format(message::kValueOutOfRangeMessage, val));
  }
  return val;
}

std::string settings::ReadStringFromNode(c4::yml::ConstNodeRef node) {
  if (node.invalid() || !node.has_val()) {
    throw winrt::hresult_invalid_argument(message::kInvalidNodeMessage);
  }

  c4::csubstr u8str = node.val();
  return {u8str.str, u8str.len};
}

std::wstring settings::ReadStringAsUtf16FromNode(c4::yml::ConstNodeRef node) {
  if (node.invalid() || !node.has_val()) {
    return L"";
  }

  c4::csubstr u8str = node.val();
  if (u8str.len == 0) {
    return L"";
  }

  std::wstring u16str;
  winrt::hresult hr = win32::ConvertUtf8ToUtf16(u8str.str, static_cast<int>(u8str.len), u16str);
  winrt::check_hresult(hr);
  return std::move(u16str);
}

settings::StringAndCompareType settings::ReadStringAndCompareTypeFromNode(c4::yml::ConstNodeRef node) {
  using CT = settings::StringCompareType;

  if (node.invalid() || !node.has_val()) {
    return {CT::kNone, L""};
  }

  c4::csubstr u8str = node.val();
  if (u8str.len == 0) {
    return {CT::kNone, L""};
  }

  CT compare_type = CT::kContains;
  char const* utf8ptr = u8str.str;
  int utf8len = static_cast<int>(u8str.len);
  if (u8str.begins_with('^')) {
    ++utf8ptr;
    --utf8len;
    compare_type |= CT::kStartsWith;
  }
  if (u8str.ends_with('$')) {
    --utf8len;
    compare_type |= CT::kEndsWith;
  }

  std::wstring u16str;
  winrt::hresult hr = win32::ConvertUtf8ToUtf16(utf8ptr, utf8len, u16str);
  winrt::check_hresult(hr);
  return {compare_type, u16str};
}
