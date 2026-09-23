#pragma once

#define _SILENCE_CXX23_DENORM_DEPRECATION_WARNING

#include <mutex>
#include <regex>

#include <boost/circular_buffer.hpp>          // use in Logger
#include <boost/container/static_vector.hpp>  // use in ListenerHost<I>
#include <c4/yml/yml.hpp>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>

#define WINAPI_PARTITION_DESKTOP 1 // for RO_INIT_SINGLETHREADED
#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>

#include <wil/cppwinrt_helpers.h>
#include "../app/utility/macro.h"
