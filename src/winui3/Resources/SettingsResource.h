#pragma once

#ifdef __INTELLISENSE__
typedef unsigned int UINT;
#endif

#if defined(RC_INVOKED) || !defined(__cplusplus)

#define IDS_SETTINGS_WINDOW_TITLE    16
#define IDS_SETTINGS_WINDOW_DEVELOP  17
#define IDS_SETTINGS_RULE_FORMAT     18

#else

namespace roxyg::winui3::viewmodel::string {
inline constexpr UINT kStringBase = 16;
inline constexpr UINT kSettingsWindowTitle = kStringBase;
inline constexpr UINT kSettingsWindowDevelop = kStringBase + 1;
inline constexpr UINT kSettingsRuleFormat = kStringBase + 2;
}

#endif
