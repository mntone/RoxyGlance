#pragma once

#if defined(__GNUC__) || defined(__clang__)
# if defined(__i386__) || defined(__arm__)
#  define ROXYG_ARCH_32BIT 1
# endif
#elifdef _MSC_VER
# if defined(_M_IX86) || defined(_M_ARM)
#  define ROXYG_ARCH_32BIT 1
# endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define ROXYG_ALWAYS_INLINE __attribute__((always_inline)) inline
#elifdef _MSC_VER
#  define ROXYG_ALWAYS_INLINE __forceinline
#else
#  define ROXYG_ALWAYS_INLINE inline
#endif
