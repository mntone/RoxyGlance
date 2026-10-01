#pragma once

#if defined(RC_INVOKED) || !defined(__cplusplus)

#define IDI_APP_MAIN  101  // Icon resource ID for the main application icon

#else

namespace roxyg::icon {
inline constexpr WORD kAppMain = 101;
}

#endif
