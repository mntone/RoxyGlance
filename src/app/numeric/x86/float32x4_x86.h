#pragma once
#include "int32x4_x86.h"

namespace roxyg::numeric {

// --- [ Utility functions ] ------------------------------

template<std::floating_point T, int Index>
  requires (sizeof(T) == 4)
NUMERIC_INLINE_CONSTEXPR T _numeric128_f32_get(__m128 val) noexcept {
  static_assert(Index >= 0 && Index <= 3, "index out of range [0-3]");
  return static_cast<T>(val.m128_f32[Index]);
}

template<std::floating_point T, int Index>
  requires (sizeof(T) == 4)
NUMERIC_INLINE_CONSTEXPR void _numeric128_f32_set(__m128& val, T scalar) noexcept {
  static_assert(Index >= 0 && Index <= 3, "index out of range [0-3]");
  val.m128_f32[Index] = static_cast<float>(scalar);
}

NUMERIC_ALWAYS_INLINE __m128 _numeric128_rint_ps(__m128 v) noexcept {
  __m128 const magic = _mm_set_ps1(8388608.f); // 2^23
  __m128 const signed_magic = _mm_or_ps(magic, _mm_and_ps(v, _mm_set_ps1(-0.f)));
  __m128 const mask = _mm_cmplt_ps(_mm_andnot_ps(_mm_set_ps1(-0.f), v), magic);
  __m128 const rounded = _mm_sub_ps(_mm_add_ps(v, signed_magic), signed_magic);
  return _mm_or_ps(_mm_and_ps(mask, rounded), _mm_andnot_ps(mask, v));
}

// --- [ float32x4 implementation ] -----------------------

template<std::floating_point T, std::size_t N>
  requires (sizeof(T) == 4 && N >= 1 && N <= 4)
struct _vec_storage<T, N, 16>: public __simd_vec_tags<T, N, 16> {
  using value_type = f32x4;
  value_type val;

  NUMERIC_INLINE_CONSTEXPR T __vectorcall x() const noexcept { return _numeric128_f32_get<T, 0>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall y() const noexcept { return _numeric128_f32_get<T, 1>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall z() const noexcept { return _numeric128_f32_get<T, 2>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall w() const noexcept { return _numeric128_f32_get<T, 3>(val); }
  NUMERIC_INLINE_CONSTEXPR T __vectorcall at(std::size_t i) const noexcept {
#if _DEBUG
    assert(i >= 0 && i <= 3 && "index out of range [0-3]");
#endif
    return static_cast<T>(val.m128_f32[i]);
  }

  NUMERIC_INLINE_CONSTEXPR _vec_storage<T, 2, 16> __vectorcall xy() const noexcept {
    _vec_storage<T, 2, 16> ret;
    NUMERIC_IF_CONSTEVAL_{
      ret.val = {val.m128_f32[0], val.m128_f32[1], 0.f, 0.f};
    } else {
      ret.val = _mm_movelh_ps(val, val);
    }
    return ret;
  }
  NUMERIC_INLINE_CONSTEXPR _vec_storage<T, 2, 16> __vectorcall zw() const noexcept {
    _vec_storage<T, 2, 16> ret;
    NUMERIC_IF_CONSTEVAL_{
      ret.val = {val.m128_f32[2], val.m128_f32[3], 0.f, 0.f};
    } else {
      ret.val = _mm_movehl_ps(val, val);
    }
    return ret;
  }

  NUMERIC_INLINE_CONSTEXPR void __vectorcall setX(T rhs) noexcept { _numeric128_f32_set<T, 0>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setY(T rhs) noexcept { _numeric128_f32_set<T, 1>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setZ(T rhs) noexcept { _numeric128_f32_set<T, 2>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setW(T rhs) noexcept { _numeric128_f32_set<T, 3>(val, rhs); }
  NUMERIC_INLINE_CONSTEXPR void __vectorcall setAt(std::size_t i, T rhs) noexcept {
#if _DEBUG
    assert(i >= 0 && i <= 3 && "index out of range [0-3]");
#endif
    val.m128_f32[i] = static_cast<float>(rhs);
  }

  NUMERIC_ALWAYS_INLINE void __vectorcall setXY(_vec_storage<T, 2, 16> rhs) noexcept {
    val = _mm_shuffle_ps(rhs.val, val, _MM_SHUFFLE(3, 2, 1, 0));
  }
  NUMERIC_ALWAYS_INLINE void __vectorcall setZW(_vec_storage<T, 2, 16> rhs) noexcept {
    val = _mm_movelh_ps(val, rhs.val);
  }

  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall splat(T x) noexcept {
    _vec_storage ret;
    ret.val = _mm_set_ps1(x);
    return ret;
  }
  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall make(T x) noexcept {
    _vec_storage ret;
    ret.val = _mm_set_ps(0.f, 0.f, 0.f, x);
    return ret;
  }
  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall make(T x, T y) noexcept {
    _vec_storage ret;
    ret.val = _mm_set_ps(0.f, 0.f, y, x);
    return ret;
  }
  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall make(T x, T y, T z) noexcept {
    _vec_storage ret;
    ret.val = _mm_set_ps(0.f, z, y, x);
    return ret;
  }
  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall make(T x, T y, T z, T w) noexcept {
    _vec_storage ret;
    ret.val = _mm_set_ps(w, z, y, x);
    return ret;
  }
  static NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall concat(_vec_storage<T, 2, 16> xy, _vec_storage<T, 2, 16> zw) noexcept {
    _vec_storage ret;
    ret.val = _mm_movelh_ps(xy.val, zw.val);
    return ret;
  }

  friend NUMERIC_ALWAYS_INLINE bool operator==(_vec_storage lhs, _vec_storage rhs) noexcept {
    return _mm_movemask_ps(_mm_cmpeq_ps(lhs.val, rhs.val)) == 0xF;
  }
  friend NUMERIC_ALWAYS_INLINE bool operator!=(_vec_storage lhs, _vec_storage rhs) noexcept {
    return _mm_movemask_ps(_mm_cmpeq_ps(lhs.val, rhs.val)) != 0xF;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall operator-() const noexcept {
    _vec_storage ret;
    ret.val = _mm_xor_ps(val, _mm_set_ps1(-0.f));
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall operator~() const noexcept {
    __m128i v = _mm_castps_si128(val);
    _vec_storage ret;
    ret.val = _mm_castsi128_ps(_mm_xor_si128(v, _mm_cmpeq_epi32(v, v)));
    return ret;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator+=(_vec_storage rhs) noexcept {
    val = _mm_add_ps(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator-=(_vec_storage rhs) noexcept {
    val = _mm_sub_ps(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator*=(_vec_storage rhs) noexcept {
    val = _mm_mul_ps(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator/=(_vec_storage rhs) noexcept {
    val = _mm_div_ps(val, rhs.val);
    return *this;
  }

  template<std::signed_integral U>
    requires (sizeof(U) == 4)
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator*=(U rhs) noexcept {
    if constexpr (__simd_feature_tags::has_avx) {
      val = _mm_mul_ps(val, _mm_broadcastss_ps(_mm_cvtsi32_ss(_mm_setzero_ps(), rhs)));
    } else {
      val = _mm_mul_ps(val, _mm_cvtepi32_ps(_mm_set1_epi32(rhs)));
    }
    return *this;
  }
  template<std::signed_integral U>
    requires (sizeof(U) == 4)
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator*=(_vec_storage<U, N, 16> rhs) noexcept {
    val = _mm_mul_ps(val, _mm_cvtepi32_ps(rhs.val));
    return *this;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator&=(_vec_storage rhs) noexcept {
    val = _mm_and_ps(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator|=(_vec_storage rhs) noexcept {
    val = _mm_or_ps(val, rhs.val);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage& __vectorcall operator^=(_vec_storage rhs) noexcept {
    val = _mm_xor_ps(val, rhs.val);
    return *this;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall ceil() const noexcept {
    _vec_storage ret;
    if constexpr (__simd_feature_tags::has_sse4_1) {
      ret.val = _mm_ceil_ps(val);
    } else {
      __m128 const rint_val = _numeric128_rint_ps(val);
      __m128 const mask = _mm_cmplt_ps(rint_val, val);
      __m128 const adjustment = _mm_and_ps(mask, _mm_set_ps1(1.f));
      ret.val = _mm_add_ps(rint_val, adjustment);  // +1 if (rint < v)
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall floor() const noexcept {
    _vec_storage ret;
    if constexpr (__simd_feature_tags::has_sse4_1) {
      ret.val = _mm_floor_ps(val);
    } else {
      __m128 const rint_val = _numeric128_rint_ps(val);
      __m128 const mask = _mm_cmpgt_ps(rint_val, val);
      __m128 const adjustment = _mm_and_ps(mask, _mm_set_ps1(1.f));
      ret.val = _mm_sub_ps(rint_val, adjustment);  // -1 if (rint > v)
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall rint() const noexcept {
    _vec_storage ret;
    if constexpr (__simd_feature_tags::has_sse4_1) {
      ret.val = _mm_round_ps(val, _MM_FROUND_NEARBYINT);
    } else {
      ret.val = _numeric128_rint_ps(val);
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage __vectorcall trunc() const noexcept {
    _vec_storage ret;
    if constexpr (__simd_feature_tags::has_sse4_1) {
      ret.val = _mm_round_ps(val, _MM_FROUND_TRUNC);
    } else {
      __m128 const rint_val = _numeric128_rint_ps(val);
      __m128 const abs_val = _mm_andnot_ps(_mm_set_ps1(-0.f), val);
      __m128 const abs_rint = _mm_andnot_ps(_mm_set_ps1(-0.f), rint_val);
      __m128 const mask = _mm_cmpgt_ps(abs_rint, abs_val);

      __m128 const sign = _mm_and_ps(val, _mm_set_ps1(-0.f));
      __m128 const unit = _mm_or_ps(_mm_set_ps1(1.f), sign);
      __m128 const adjustment = _mm_and_ps(mask, unit);
      ret.val = _mm_sub_ps(rint_val, adjustment);  // ±1 if (|rint| > |v|)
    }
    return ret;
  }

  static constexpr std::size_t long_storage_length = __storage_size<long, _vec_storage<T, N, 16>>();
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> __vectorcall _lceil() const noexcept {
    _vec_storage<long, N, long_storage_length> ret;
    if constexpr (__simd_feature_tags::has_sse4_1) {
      ret.val = _mm_cvttps_epi32(_mm_ceil_ps(val));
    } else {
      __scoped_mxcsr guard(_MM_ROUND_UP);
      ret.val = _mm_cvtps_epi32(val);
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> __vectorcall _lfloor() const noexcept {
    _vec_storage<long, N, long_storage_length> ret;
    if constexpr (__simd_feature_tags::has_sse4_1) {
      ret.val = _mm_cvttps_epi32(_mm_floor_ps(val));
    } else {
      __scoped_mxcsr guard(_MM_ROUND_DOWN);
      ret.val = _mm_cvtps_epi32(val);
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> __vectorcall _lrint() const noexcept {
    _vec_storage<long, N, long_storage_length> ret;
    if constexpr (__simd_feature_tags::has_sse4_1) {
      ret.val = _mm_cvttps_epi32(_mm_round_ps(val, _MM_FROUND_NEARBYINT));
    } else {
      __scoped_mxcsr guard(_MM_ROUND_NEAREST);
      ret.val = _mm_cvtps_epi32(val);
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> __vectorcall _lround() const noexcept {
    _vec_storage<long, N, long_storage_length> ret;
    {
      f32x4 const sign_mask = _mm_and_ps(val, _mm_set1_ps(-0.f));
      f32x4 const bias = _mm_or_ps(_mm_set1_ps(0.5f), sign_mask);
      ret.val = _mm_cvttps_epi32(_mm_add_ps(val, bias));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> __vectorcall _ltrunc() const noexcept {
    _vec_storage<long, N, long_storage_length> ret;
    ret.val = _mm_cvttps_epi32(val);
    return ret;
  }

  template<std::signed_integral U>
    requires (sizeof(U) == 4)
  explicit NUMERIC_ALWAYS_INLINE __vectorcall operator _vec_storage<U, N, 16>() const noexcept {
    _vec_storage<U, N, 16> ret;
    ret.val = _mm_cvttps_epi32(val);
    return ret;
  }
};

}  // namespace roxyg::numeric
