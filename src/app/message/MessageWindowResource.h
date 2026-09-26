#pragma once

#if defined(RC_INVOKED) || !defined(__cplusplus)

#define IDC_MENU_TRAY      103    // Menu resource ID for the tray icon context menu
#define IDM_TRAY_EXIT      40301  // Menu item ID for "E&xit" in the tray menu
#define IDM_TRAY_SETTINGS  40302  // Menu item ID for "&Settings" in the tray menu
#define IDM_TRAY_DEBUG     40303  // Menu item ID for "&Debug" in the tray menu
#define IDS_MENU_DEBUG     10301

#else

namespace roxyg::message {

namespace menu {
inline constexpr WORD kTray = 103;
}

namespace command {
inline constexpr WORD kTrayCommandBase = 40300;
inline constexpr WORD kTrayExit = kTrayCommandBase + 1;
inline constexpr WORD kTraySettings = kTrayCommandBase + 2;
inline constexpr WORD kTrayDebug = kTrayCommandBase + 3;
}

namespace string {
inline constexpr WORD kStringBase = 10300;
inline constexpr WORD kTrayDebug = kStringBase + 1;
}

}

#endif
