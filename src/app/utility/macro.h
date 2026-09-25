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

#ifdef _DEBUG
# include <cassert>
# define ROXYG_UNCHECKED_ASSERT(expr) assert(expr)
#else
# if defined(__cpp_attribute_assume) && __cpp_attribute_assume >= 202207L
#  define ROXYG_UNCHECKED_ASSERT(expr) [[assume(expr)]]
# elif defined(__has_cpp_attribute) && __has_cpp_attribute(assume)
#  define ROXYG_UNCHECKED_ASSERT(expr) [[assume(expr)]]
# elifdef __GNUC__
#  define ROXYG_UNCHECKED_ASSERT(expr) do { if (!(expr)) { __builtin_unreachable(); }} while(0)
# elifdef __clang__
#  define ROXYG_UNCHECKED_ASSERT(expr) __builtin_assume(expr)
# elifdef _MSC_VER
#  define ROXYG_UNCHECKED_ASSERT(expr) __assume(expr)
# else
#  define ROXYG_UNCHECKED_ASSERT(expr) ((void)0)
# endif
#endif
