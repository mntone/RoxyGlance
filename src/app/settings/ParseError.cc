#include "pch.h"
#include "ParseErrorPrivate.h"

#include "constants.h"
#include "../win32/hresult.h"
#include "../win32/string.h"

namespace {
inline constexpr std::wstring_view kMissingRequiredKeyMessage = L"Required key \"{}\" is missing in rule {}";
inline constexpr std::wstring_view kExpectedStringMessage = L"Key \"{}\" must be a string in rule {}";
inline constexpr std::wstring_view kExpectedNumberMessage = L"Key \"{}\" must be a number in rule {}";
inline constexpr std::wstring_view kExpectedMapMessage = L"Key \"{}\" must be a map in rule {}";
inline constexpr std::wstring_view kExpectedSequenceMessage = L"Key \"{}\" must be a sequence in rule {}";
inline constexpr std::wstring_view kInvalidStringMessage = L"Key \"{}\" has an invalid string in rule {}";
inline constexpr std::wstring_view kInvalidNumberMessage = L"Key \"{}\" has an invalid number in rule {}";
inline constexpr std::wstring_view kNumberOutOfRangeMessage = L"Key \"{}\" is out of range in rule {}: {}";
}

using namespace roxyg::settings;

static ROXYG_ALWAYS_INLINE constexpr std::wstring_view convertKeyName(KeyId value) noexcept {
  using namespace std::string_view_literals;

  switch (value) {
  case KeyId::kRule:  return L"rules[]"sv;
  case KeyId::kName:  return L"name"sv;
  case KeyId::kWhen:  return L"when"sv;
  case KeyId::kWhere: return L"where"sv;
  case KeyId::kThen:  return L"then"sv;

  case KeyId::kProcessImageName: return L"process"sv;
  case KeyId::kWindowClass:      return L"class"sv;
  case KeyId::kWindowTitle:      return L"title"sv;

  case KeyId::kActionType:  return L"type"sv;
  case KeyId::kPositionX:   return L"x"sv;
  case KeyId::kPositionY:   return L"y"sv;
  case KeyId::kWidth:       return L"width"sv;
  case KeyId::kHeight:      return L"height"sv;
  case KeyId::kMonitorId:   return L"id"sv;
  case KeyId::kMonitorName: return L"name"sv;

  default: return L"?"sv;
  }
}

static ROXYG_ALWAYS_INLINE std::wstring convertString(char const* u8ptr, size_t u8len) {
  std::wstring u16str;
  if (u8len > 0) {
    winrt::hresult const hr = roxyg::win32::ConvertUtf8ToUtf16(u8ptr, static_cast<int>(u8len), u16str);
    winrt::check_hresult(hr);
  }
  return u16str;
}
static ROXYG_ALWAYS_INLINE std::wstring convertString(c4::csubstr str) {
  return convertString(str.data(), str.size());
}

std::wstring detail::makeRuleDisplayName(c4::yml::ConstNodeRef rule, ParseError const err, size_t const index) {
  std::wstring name;
  if (err.key_id == KeyId::kRule  // Thrown only when the rule is not a map.
      || !rule.has_child(key::kName)) {
    name = std::format(L"#{}", index);
  } else {
    c4::yml::ConstNodeRef name_node{rule[key::kName]};
    if (name_node.is_keyval() && !name_node.val_is_null()) {
      try {
        name = convertString(name_node.val());
        name.insert(0, 1, L'"');
        name.push_back(L'"');
      } catch (winrt::hresult_error const& conv_err) {
        if (conv_err.code() == roxyg::win32::hresult::kErrorNoUnicodeTranslation) {
          name = std::format(L"#{}", index);
        } else {
          throw;
        }
      }
    } else {
      name = std::format(L"#{}", index);
    }
  }
  return name;
}

winrt::hstring detail::makeParseErrorMessage(ParseError err, std::wstring_view rule_name) {
  std::wstring_view const key_name{convertKeyName(err.key_id)};

  winrt::hstring message;
  switch (err.reason) {
  case ParseErrorReason::kMissingRequiredKey:
    message = winrt::format(kMissingRequiredKeyMessage, key_name, rule_name);
    break;
  case ParseErrorReason::kExpectedString:
    message = winrt::format(kExpectedStringMessage, key_name, rule_name);
    break;
  case ParseErrorReason::kExpectedNumber:
    message = winrt::format(kExpectedNumberMessage, key_name, rule_name);
    break;
  case ParseErrorReason::kExpectedMap:
    message = winrt::format(kExpectedMapMessage, key_name, rule_name);
    break;
  case ParseErrorReason::kExpectedSequence:
    message = winrt::format(kExpectedSequenceMessage, key_name, rule_name);
    break;
  case ParseErrorReason::kInvalidString:
    message = winrt::format(kInvalidStringMessage, key_name, rule_name);
    break;
  case ParseErrorReason::kInvalidNumber:
    message = winrt::format(kInvalidNumberMessage, key_name, rule_name);
    break;
  case ParseErrorReason::kNumberOutOfRange:
    message = winrt::format(
      kNumberOutOfRangeMessage,
      key_name,
      rule_name,
      convertString(err.content, std::char_traits<char>::length(err.content))
    );
    break;
  }
  return message;
}
