#ifndef RC_INVOKED
#define RC_INVOKED
#endif

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

#ifdef ROXYG_VER_FILE_VERSION
#define VER_FILE_VERSION            ROXYG_VER_FILE_VERSION
#else
#define VER_FILE_VERSION            0,0,0,0
#endif
#ifdef ROXYG_VER_FILE_VERSION_STRING
#define VER_FILE_VERSION_STRING     STRINGIZE(ROXYG_VER_FILE_VERSION_STRING)
#else
#define VER_FILE_VERSION_STRING     "0.0.0.0"
#endif
#ifdef ROXYG_VER_PRODUCT_VERSION
#define VER_PRODUCT_VERSION         ROXYG_VER_PRODUCT_VERSION
#else
#define VER_PRODUCT_VERSION         0,0,0,0
#endif
#ifdef ROXYG_VER_PRODUCT_VERSION_STRING
#define VER_PRODUCT_VERSION_STRING  STRINGIZE(ROXYG_VER_PRODUCT_VERSION_STRING)
#else
#define VER_PRODUCT_VERSION_STRING  "0.0.0-unknown"
#endif

#include "../../app/message/MessageWindowResource.h"  // 103, 403xx is reserved.

// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        104
#define _APS_NEXT_COMMAND_VALUE         40400
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           104
#endif
#endif
