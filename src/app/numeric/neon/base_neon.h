#pragma once
#include "../common.h"

#ifdef _M_ARM
#include <arm_neon.h>
#else
#include <arm64_neon.h>  // has any of _M_ARM64, _M_HYBRID_X86_ARM64, _M_ARM64EC
#endif

namespace roxyg::numeric {

struct __simd_feature_tags {
#ifdef _M_ARM
  static constexpr bool is_64bit = false;
#else
  static constexpr bool is_64bit = true;
#endif
  static constexpr bool has_ssse3 = false;
  static constexpr bool has_avx = false;
  static constexpr bool has_avx2 = false;
};

using i32x4 = int32x4_t;
using f128 = float32x4_t;

struct _nn128_vec_tags {
  static constexpr bool enable_simd = true;
  static constexpr std::size_t simd_bits = 128;
};

}
