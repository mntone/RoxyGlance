#pragma once
#include "base_x86.h"

namespace roxyg::numeric {

// --- [ Utility functions ] ------------------------------

template<std::signed_integral T, int Index>
  requires (sizeof(T) == 4)
NUMERIC_ALWAYS_INLINE T _numeric128_i32_get(__m128i val) {
  static_assert(Index >= 0 && Index <= 3, "index out of range [0-3]");
  return static_cast<T>(val.m128i_i32[Index]);
}

template<std::signed_integral T, int Index>
  requires (sizeof(T) == 4)
NUMERIC_ALWAYS_INLINE void _numeric128_i32_set(__m128i& val, T scalar) {
  static_assert(Index >= 0 && Index <= 3, "index out of range [0-3]");
  val.m128i_i32[Index] = static_cast<int>(scalar);
}


// --- [ int32x4 implementation ] -------------------------

template<std::signed_integral T, std::size_t N>
  requires (sizeof(T) == 4 && N >= 1 && N <= 4)
struct _vec_storage<T, N, 16>: public __simd_vec_tags<T, N, 16> {
  using value_type = i32x4;
  value_type val;

  [[nodiscard]] NUMERIC_ALWAYS_INLINE T x() const noexcept requires (N >= 1) { return _numeric128_i32_get<T, 0>(val); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T y() const noexcept requires (N >= 2) { return _numeric128_i32_get<T, 1>(val); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T z() const noexcept requires (N >= 3) { return _numeric128_i32_get<T, 2>(val); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T w() const noexcept requires (N == 4) { return _numeric128_i32_get<T, 3>(val); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T at(std::size_t i) const noexcept {
#if _DEBUG
    assert(i >= 0 && i <= 3 && "index out of range [0-3]");
#endif
    return static_cast<T>(val.m128i_i32[i]);
  }

  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage<T, 2, 16> xy() const noexcept requires (N >= 2) {
    _vec_storage<T, 2, 16> ret;
    ret.val = _mm_unpacklo_epi64(val, val);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage<T, 2, 16> zw() const noexcept requires (N == 4) {
    _vec_storage<T, 2, 16> ret;
    ret.val = _mm_unpackhi_epi64(val, val);
    return ret;
  }

  NUMERIC_ALWAYS_INLINE void setX(T rhs) noexcept requires (N >= 1) { _numeric128_i32_set<T, 0>(val, rhs); }
  NUMERIC_ALWAYS_INLINE void setY(T rhs) noexcept requires (N >= 2) { _numeric128_i32_set<T, 1>(val, rhs); }
  NUMERIC_ALWAYS_INLINE void setZ(T rhs) noexcept requires (N >= 3) { _numeric128_i32_set<T, 2>(val, rhs); }
  NUMERIC_ALWAYS_INLINE void setW(T rhs) noexcept requires (N == 4) { _numeric128_i32_set<T, 3>(val, rhs); }
  NUMERIC_ALWAYS_INLINE void setAt(std::size_t i, T rhs) noexcept {
#if _DEBUG
    assert(i >= 0 && i <= 3 && "index out of range [0-3]");
#endif
    val.m128i_i32[i] = static_cast<int>(rhs);
  }

  NUMERIC_ALWAYS_INLINE void setXY(_vec_storage<T, 2, 16> rhs) noexcept requires (N >= 2) {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      val = _mm_blend_epi16(val, rhs.val, 0x0F);
    } else {
      val = _mm_unpacklo_epi64(rhs.val, _mm_unpackhi_epi64(val, val));
    }
  }
  NUMERIC_ALWAYS_INLINE void setZW(_vec_storage<T, 2, 16> rhs) noexcept requires (N == 4) {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      val = _mm_blend_epi16(val, rhs.val, 0xF0);
    } else {
      val = _mm_unpacklo_epi64(val, rhs.val);
    }
  }

  [[nodiscard]] NUMERIC_ALWAYS_INLINE static _vec_storage __vectorcall splat(T x) noexcept {
    _vec_storage ret;
    ret.val = _mm_set1_epi32(x);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static _vec_storage __vectorcall make(T x) noexcept requires (N == 1) {
    _vec_storage ret;
    ret.val = _mm_set_epi32(0, 0, 0, x);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static _vec_storage __vectorcall make(T x, T y) noexcept requires (N == 2) {
    _vec_storage ret;
    ret.val = _mm_set_epi32(0, 0, y, x);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static _vec_storage __vectorcall make(T x, T y, T z) noexcept requires (N == 3) {
    _vec_storage ret;
    ret.val = _mm_set_epi32(0, z, y, x);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static _vec_storage __vectorcall make(T x, T y, T z, T w) noexcept requires (N == 4) {
    _vec_storage ret;
    ret.val = _mm_set_epi32(w, z, y, x);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static _vec_storage __vectorcall concat(_vec_storage<T, 2, 16> xy, _vec_storage<T, 2, 16> zw) noexcept requires (N == 4) {
    _vec_storage ret;
    ret.val = _mm_unpacklo_epi64(xy.val, zw.val);
    return ret;
  }

  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE bool operator==(_vec_storage lhs, _vec_storage rhs) noexcept {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      i32x4 const diff = _mm_xor_si128(lhs.val, rhs.val);
      return _mm_testz_si128(diff, diff) != 0;
    } else {
      return _mm_movemask_epi8(_mm_cmpeq_epi32(lhs.val, rhs.val)) == 0xFFFF;
    }
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE bool operator!=(_vec_storage lhs, _vec_storage rhs) noexcept {
    if constexpr (__simd_feature_tags::has_sse4_1) {
      i32x4 const diff = _mm_xor_si128(lhs.val, rhs.val);
      return _mm_testz_si128(diff, diff) == 0;
    } else {
      return _mm_movemask_epi8(_mm_cmpeq_epi32(lhs.val, rhs.val)) != 0xFFFF;
    }
  }

  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall operator-() const noexcept {
    _vec_storage ret;
    if constexpr (__simd_feature_tags::has_ssse3) {
      ret.val = _mm_sign_epi32(val, _mm_set1_epi32(-1));
    } else {
      ret.val = _mm_sub_epi32(_mm_setzero_si128(), val);
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall operator~() const noexcept {
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
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator/=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) / rhs.at(i));
    }
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
  [[nodiscard]] explicit NUMERIC_ALWAYS_INLINE __vectorcall operator _vec_storage<U, N, 16>() const noexcept {
    _vec_storage<U, N, 16> ret;
    ret.val = _mm_cvtepi32_ps(val);
    return ret;
  }
};

}  // namespace roxyg::numeric
