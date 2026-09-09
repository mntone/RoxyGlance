#pragma once

namespace roxyg::settings {

inline constexpr std::wstring_view kUserSettingsFileName = L"\\roxyg_user.yaml";

namespace key {

// *
inline constexpr c4::csubstr kRules = "rules";  // std::vector<Rule>

// rules[].*
inline constexpr c4::csubstr kWhen = "when";    // TriggerType
inline constexpr c4::csubstr kWhere = "where";  // Filter
inline constexpr c4::csubstr kThen = "then";    // Action

// rules[].where.*
inline constexpr c4::csubstr kProcessKey = "process";
inline constexpr c4::csubstr kWindowClassKey = "class";
inline constexpr c4::csubstr kWindowTitleKey = "title";

// rules[].then.*
inline constexpr c4::csubstr kName = "name";
inline constexpr c4::csubstr kType = "type";
inline constexpr c4::csubstr kPosX = "x";
inline constexpr c4::csubstr kPosY = "y";
inline constexpr c4::csubstr kWidth = "width";
inline constexpr c4::csubstr kHeight = "height";

}  // namespace key

namespace message {

inline constexpr std::wstring_view kInvalidFormatMessage = L"invalid yaml format";
inline constexpr std::wstring_view kInvalidNodeMessage = L"invalid node";
inline constexpr std::wstring_view kInvalidNumberMessage = L"invalid number";
inline constexpr std::wstring_view kMissingRequiredKeyMessage = L"missing required key";
inline constexpr std::wstring_view kValueOutOfRangeMessage = L"value out of range: {}";

}  // namespace message

}
