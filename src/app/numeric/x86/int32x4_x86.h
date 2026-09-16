#pragma once
#include "base_x86.h"

namespace roxyg::numeric {

// --- [ Utility functions ] ------------------------------

template<std::signed_integral T, int Index>
  requires (sizeof(T) == 4)
NUMERIC_INLINE_CONSTEXPR T _numeric128_i32_get(__m128i val) noexcept {
  static_assert(Index >= 0 && Index <= 3, "index out of range [0-3]");
  return static_cast<T>(val.m128i_i32[Index]);
}

template<std::signed_integral T, int Index>
  requires (sizeof(T) == 4)
NUMERIC_INLINE_CONSTEXPR void _numeric128_i32_set(__m128i& val, T scalar) noexcept {
  static_assert(Index >= 0 && Index <= 3, "index out of range [0-3]");
  val.m128i_i32[Index] = static_cast<int>(scalar);
}


// --- [ int32x4 implementation ] -------------------------

template<std::signed_integral T, std::size_t N>
  requires (sizeof(T) == 4 && N >= 1 && N <= 4)
struct _vec_storage<T, N, 16>: public __simd_vec_tags<T, N, 16> {
  using value_type = i32x4;
  value_type val;

  NUMERIC_INLINE_CONSTEXPR T __vectorcall x() const noexcept { return _numeric128_i32_get<T, 0>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall y() const noexcept { return _numeric128_i32_get<T, 1>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall z() const noexcept { return _numeric128_i32_get<T, 2>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall w() const noexcept { return _numeric128_i32_get<T, 3>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall at(std::size_t i) const noexcept {
#if _DEBUG
    assert(i >= 0 && i <= 3 && "index out of range [0-3]");
#endif
    return static_cast<T>(val.m128i_i32[i]);
  }

  NUMERIC_INLINE_CONSTEXPR _vec_storage<T, 2, 16> __vectorcall xy() const noexcept {
    _vec_storage<T, 2, 16> ret;
    NUMERIC_IF_CONSTEVAL_{
      ret.val = {.m128i_i32 = {val.m128i_i32[0], val.m128i_i32[1], 0, 0}};
    } else {
      ret.val = _mm_unpacklo_epi64(val, val);
    }
    return ret;
  }
  NUMERIC_INLINE_CONSTEXPR _vec_storage<T, 2, 16> __vectorcall zw() const noexcept {
    _vec_storage<T, 2, 16> ret;
    NUMERIC_IF_CONSTEVAL_{
      ret.val = {.m128i_i32 = {val.m128i_i32[2], val.m128i_i32[3], 0, 0}};
    } else {
      ret.val = _mm_unpackhi_epi64(val, val);
    }
    return ret;
  }

  NUMERIC_INLINE_CONSTEXPR void __vectorcall setX(T rhs) noexcept { _numeric128_i32_set<T, 0>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setY(T rhs) noexcept { _numeric128_i32_set<T, 1>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setZ(T rhs) noexcept { _numeric128_i32_set<T, 2>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setW(T rhs) noexcept { _numeric128_i32_set<T, 3>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setAt(std::size_t i, T rhs) noexcept {
#if _DEBUG
    assert(i >= 0 && i <= 3 && "index out of range [0-3]");
#endif
    val.m128i_i32[i] = static_cast<int>(rhs);
  }

  NUMERIC_ALWAYS_INLINE void __vectorcall setXY(_vec_storage<T, 2, 16> rhs) noexcept {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      val = _mm_blend_epi16(val, rhs.val, 0x0F);
    } else {
      val = _mm_unpacklo_epi64(rhs.val, _mm_unpackhi_epi64(val, val));
    }
  }
  NUMERIC_ALWAYS_INLINE void __vectorcall setZW(_vec_storage<T, 2, 16> rhs) noexcept {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      val = _mm_blend_epi16(val, rhs.val, 0xF0);
    } else {
      val = _mm_unpacklo_epi64(val, rhs.val);
    }
  }

  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall splat(T x) noexcept {
    _vec_storage ret;
    if constexpr (N == 1) {
      ret.val = _mm_cvtsi32_si128(x);
    } else if constexpr (N == 2) {
      if constexpr (__simd_feature_tags::has_sse4_1) {
        ret.val = _mm_blend_epi16(_mm_setzero_si128(), _mm_set1_epi32(x), 0x0F);
      } else {
        ret.val = _mm_shuffle_epi32(_mm_cvtsi32_si128(x), _MM_SHUFFLE(1, 1, 0, 0));
      }
    } else if constexpr (N == 3) {
      if constexpr (__simd_feature_tags::has_sse4_1) {
        ret.val = _mm_blend_epi16(_mm_setzero_si128(), _mm_set1_epi32(x), 0x3F);
      } else {
        __m128i single = _mm_cvtsi32_si128(x);
        ret.val = _mm_unpacklo_epi32(_mm_shuffle_epi32(single, _MM_SHUFFLE(1, 1, 0, 0)), single);
      }
    } else {
      ret.val = _mm_set1_epi32(x);
    }
    return ret;
  }
  static NUMERIC_INLINE_CONSTEXPR _vec_storage __vectorcall make(T x, T y, T z, T w) noexcept {
    _vec_storage ret;
    NUMERIC_IF_CONSTEVAL_{
      ret.val = {.m128i_i32 = {x, y, z, w}};
    } else {
      ret.val = _mm_set_epi32(w, z, y, x);
    }
    return ret;
  }
  static NUMERIC_INLINE_CONSTEXPR _vec_storage __vectorcall make(T x, T y, T z) noexcept {
    return make(x, y, z, 0);
  }
  static NUMERIC_INLINE_CONSTEXPR _vec_storage __vectorcall make(T x, T y) noexcept {
    return make(x, y, 0, 0);
  }
  static NUMERIC_INLINE_CONSTEXPR _vec_storage __vectorcall make(T x) noexcept {
    _vec_storage ret;
    NUMERIC_IF_CONSTEVAL_{
      ret.val = {.m128i_i32 = {x, 0, 0, 0}};
    } else {
      ret.val = _mm_cvtsi32_si128(x);
    }
    return ret;
  }
  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall concat(_vec_storage<T, 2, 16> xy, _vec_storage<T, 2, 16> zw) noexcept {
    _vec_storage ret;
    ret.val = _mm_unpacklo_epi64(xy.val, zw.val);
    return ret;
  }

  friend NUMERIC_ALWAYS_INLINE bool operator==(_vec_storage lhs, _vec_storage rhs) noexcept {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      i32x4 const diff = _mm_xor_si128(lhs.val, rhs.val);
      return _mm_testz_si128(diff, diff) != 0;
    } else {
      return _mm_movemask_epi8(_mm_cmpeq_epi32(lhs.val, rhs.val)) == 0xFFFF;
    }
  }
  friend NUMERIC_ALWAYS_INLINE bool operator!=(_vec_storage lhs, _vec_storage rhs) noexcept {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      i32x4 const diff = _mm_xor_si128(lhs.val, rhs.val);
      return _mm_testz_si128(diff, diff) == 0;
    } else {
      return _mm_movemask_epi8(_mm_cmpeq_epi32(lhs.val, rhs.val)) != 0xFFFF;
    }
  }

  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall operator-() const noexcept {
    _vec_storage ret;
    if constexpr (__simd_feature_tags::has_ssse3) {
      ret.val = _mm_sign_epi32(val, _mm_set1_epi32(-1));
    } else {
      ret.val = _mm_sub_epi32(_mm_setzero_si128(), val);
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall operator~() const noexcept {
    _vec_storage ret;
    ret.val = _mm_xor_si128(val, _mm_cmpeq_epi32(val, val));
    return ret;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator+=(_vec_storage rhs) noexcept {
    val = _mm_add_epi32(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator-=(_vec_storage rhs) noexcept {
    val = _mm_sub_epi32(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator*=(_vec_storage rhs) noexcept {
    val = _mm_mullo_epi32(val, rhs.val);
    return *this;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator&=(_vec_storage rhs) noexcept {
    val = _mm_and_si128(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator|=(_vec_storage rhs) noexcept {
    val = _mm_or_si128(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator^=(_vec_storage rhs) noexcept {
    val = _mm_xor_si128(val, rhs.val);
    return *this;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator>>=(_vec_storage rhs) noexcept {
    if constexpr (__simd_feature_tags::has_avx2) {
      val = _mm_srav_epi32(val, rhs.val);
    } else {
      for (std::size_t i = 0; i < N; ++i) {
        setAt(i, at(i) >> rhs.at(i));
      }
    }
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator<<=(_vec_storage rhs) noexcept {
    if constexpr (__simd_feature_tags::has_avx2) {
      val = _mm_sllv_epi32(val, rhs.val);
    } else {
      for (std::size_t i = 0; i < N; ++i) {
        setAt(i, at(i) << rhs.at(i));
      }
    }
    return *this;
  }

  template<std::floating_point U>
    requires (sizeof(U) == 4)
  explicit NUMERIC_ALWAYS_INLINE __vectorcall operator _vec_storage<U, N, 16>() const noexcept {
    _vec_storage<U, N, 16> ret;
    ret.val = _mm_cvtepi32_ps(val);
    return ret;
  }
};

}  // namespace roxyg::numeric
