#pragma once

#ifdef __INTELLISENSE__
typedef unsigned int UINT;
#endif

#if defined(RC_INVOKED) || !defined(__cplusplus)

#define IDS_SHELL_MAIN         0
#define IDS_SHELL_MAINFMT      1
#define IDS_SHELL_DISPFMT      2
#define IDS_SHELL_DISPNAMEFMT  3

#else

namespace roxyg::winui3::string {
inline constexpr UINT kStringBase = 0;
inline constexpr UINT kShellMain = kStringBase;
inline constexpr UINT kShellMainFormat = kStringBase + 1;
inline constexpr UINT kShellDisplayFormat = kStringBase + 2;
inline constexpr UINT kShellDisplayNameFormat = kStringBase + 3;
}

#endif
