#pragma once
#include "../common.h"

#ifdef __AVX2__
#include <immintrin.h>
#else
#include <emmintrin.h>
#endif

namespace roxyg::numeric {

struct __simd_feature_tags {
#ifdef _M_IX86
  static constexpr bool is_64bit = false;
#else
  static constexpr bool is_64bit = true;
#endif

#ifdef __AVX__
  static constexpr bool has_ssse3 = true;
  static constexpr bool has_sse4_1 = true;
  static constexpr bool has_avx = true;
#  ifdef __AVX2__
  static constexpr bool has_avx2 = true;
#  else
  static constexpr bool has_avx2 = false;
#  endif
#else
#  ifdef __SSSE3__
  static constexpr bool has_ssse3 = true;
#    ifdef __SSE4_1__
  static constexpr bool has_sse4_1 = true;
#    else
  static constexpr bool has_sse4_1 = false;
#    endif
#  else
  static constexpr bool has_ssse3 = false;
  static constexpr bool has_sse4_1 = false;
#  endif
  static constexpr bool has_avx = false;
  static constexpr bool has_avx2 = false;
#endif
};

using i32x4 = __m128i;
using f32x4 = __m128;

struct __scoped_mxcsr final {
  unsigned int old_csr;

  NUMERIC_ALWAYS_INLINE explicit __scoped_mxcsr(unsigned int new_rounding_mode) noexcept {
    old_csr = _mm_getcsr();
    _MM_SET_ROUNDING_MODE(new_rounding_mode);
  }

  ~__scoped_mxcsr() noexcept {
    _mm_setcsr(old_csr);
  }

private:
  __scoped_mxcsr(__scoped_mxcsr const&) = delete;
  __scoped_mxcsr(__scoped_mxcsr&&) = delete;
  __scoped_mxcsr& operator=(__scoped_mxcsr const&) = delete;
  __scoped_mxcsr& operator=(__scoped_mxcsr&&) = delete;
};

}
