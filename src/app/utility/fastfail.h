#pragma once
#include "macro.h"

#ifdef _MSC_VER
# include <intrin.h>
# include <winnt.h>
#elif !defined(__GNUC__) && !defined(__clang__)
# include <cstdlib>
#endif

namespace roxyg::utility {

[[noreturn]] ROXYG_ALWAYS_INLINE void fastfail() {
#if defined(__GNUC__) || defined(__clang__)
  __builtin_trap();
#elifdef _MSC_VER
  __fastfail(FAST_FAIL_FATAL_APP_EXIT);
#else
  std::abort();
#endif
}

}
