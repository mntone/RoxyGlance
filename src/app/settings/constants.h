#pragma once

namespace roxyg::settings {

inline constexpr std::wstring_view kUserSettingsFileName = L"\\roxyg_user.yaml";

namespace key {

// *
inline constexpr c4::csubstr kRules = "rules";  // std::vector<Rule>

// rules[].*
inline constexpr c4::csubstr kName = "name";
inline constexpr c4::csubstr kWhen = "when";    // TriggerFlags
inline constexpr c4::csubstr kWhere = "where";  // Filter
inline constexpr c4::csubstr kThen = "then";    // Action

// rules[].where.*
inline constexpr c4::csubstr kProcessKey = "process";
inline constexpr c4::csubstr kWindowClassKey = "class";
inline constexpr c4::csubstr kWindowTitleKey = "title";

// rules[].then.*
inline constexpr c4::csubstr kType = "type";
inline constexpr c4::csubstr kMonitorId = "id";
inline constexpr c4::csubstr kMonitorName = "name";
inline constexpr c4::csubstr kPosX = "x";
inline constexpr c4::csubstr kPosY = "y";
inline constexpr c4::csubstr kWidth = "width";
inline constexpr c4::csubstr kHeight = "height";

}  // namespace key

namespace message {

inline constexpr std::wstring_view kInvalidNodeMessage = L"invalid node";

}  // namespace message

namespace numeric_limit {

inline constexpr long kAbsoluteCoordinateMin = 0;
inline constexpr long kAbsoluteCoordinateMax = 0x4000;
inline constexpr int kRelativeMonitorIdUnset = 0;
inline constexpr int kRelativeMonitorIdMin = 1;
inline constexpr int kRelativeMonitorIdMax = 16;
inline constexpr float kRelativeCoordinateMin = 0.f;
inline constexpr float kRelativeCoordinateMax = 1.f;

}

}
