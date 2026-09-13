#pragma once

#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)
#include "./neon/base_neon.h"
#elif defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64)
#include "./x86/float32x4_x86.h"
#else
#include "./common.h"
#endif

#pragma warning(push)
#pragma warning(disable:4324)

namespace roxyg::numeric {

template<arithmetic T, std::size_t N, std::size_t Align = std::bit_ceil(sizeof(T)* N)>
struct alignas(__alignup(sizeof(T), Align)) vec {
  using storage_type = _vec_storage<T, N, Align>;
  using value_type = storage_type::value_type;

  storage_type storage;

  [[nodiscard]] NUMERIC_ALWAYS_INLINE T x() const noexcept requires (N >= 1) { return storage.x(); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T y() const noexcept requires (N >= 2) { return storage.y(); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T z() const noexcept requires (N >= 3) { return storage.z(); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T w() const noexcept requires (N >= 4) { return storage.w(); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T at(std::size_t i) const noexcept { return storage.at(i); }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE T operator[](std::size_t i) const { return storage.at(i); }

  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec<T, 2, Align> xy() const noexcept requires (N >= 2) { return {storage.xy()}; }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec<T, 2, Align> zw() const noexcept requires (N >= 4) { return {storage.zw()}; }

  NUMERIC_ALWAYS_INLINE void setX(T rhs) noexcept requires (N >= 1) { storage.setX(rhs); }
  NUMERIC_ALWAYS_INLINE void setY(T rhs) noexcept requires (N >= 2) { storage.setY(rhs); }
  NUMERIC_ALWAYS_INLINE void setZ(T rhs) noexcept requires (N >= 3) { storage.setZ(rhs); }
  NUMERIC_ALWAYS_INLINE void setW(T rhs) noexcept requires (N >= 4) { storage.setW(rhs); }
  NUMERIC_ALWAYS_INLINE void setAt(std::size_t i, T rhs) noexcept { storage.setAt(i, rhs); }

  [[nodiscard]] NUMERIC_ALWAYS_INLINE void setXY(vec<T, 2, Align> rhs) noexcept requires (N >= 2) {
    storage.setXY(rhs.storage);
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE void setZW(vec<T, 2, Align> rhs) noexcept requires (N >= 4) {
    storage.setZW(rhs.storage);
  }

  [[nodiscard]] NUMERIC_ALWAYS_INLINE static vec splat(T s) noexcept {
    vec ret;
    ret.storage = storage_type::splat(s);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static vec make(T x) noexcept requires (N == 1) {
    vec ret;
    ret.storage = storage_type::make(x);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static vec make(T x, T y) noexcept requires (N == 2) {
    vec ret;
    ret.storage = storage_type::make(x, y);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static vec make(T x, T y, T z) noexcept requires (N == 3) {
    vec ret;
    ret.storage = storage_type::make(x, y, z);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static vec make(T x, T y, T z, T w) noexcept requires (N == 4) {
    vec ret;
    ret.storage = storage_type::make(x, y, z, w);
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE static vec<T, 4> concat(vec<T, 2, 16> xy, vec<T, 2, 16> zw) noexcept requires (N == 4) {
    vec ret;
    ret.storage = storage_type::concat(xy.storage, zw.storage);
    return ret;
  }

  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE bool operator==(vec lhs, vec rhs) noexcept {
    return lhs.storage == rhs.storage;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE bool operator!=(vec lhs, vec rhs) noexcept {
    return lhs.storage != rhs.storage;
  }

  [[nodiscard]] constexpr vec operator+() const noexcept {
    return *this;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec operator-() const noexcept requires (std::signed_integral<T> || std::floating_point<T>) {
    vec ret;
    ret.storage = -storage;
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec operator~() const noexcept {
    vec ret;
    ret.storage = ~storage;
    return ret;
  }

  NUMERIC_ALWAYS_INLINE vec& operator+=(T rhs) noexcept {
    storage += storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator+=(vec rhs) noexcept {
    storage += rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator+(vec lhs, T rhs) noexcept {
    lhs += rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator+(T lhs, vec rhs) noexcept {
    rhs += lhs;
    return rhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator+(vec lhs, vec rhs) noexcept {
    lhs += rhs;
    return lhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator-=(T rhs) noexcept {
    storage -= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator-=(vec rhs) noexcept {
    storage -= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator-(vec lhs, T rhs) noexcept {
    lhs -= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator-(vec lhs, vec rhs) noexcept {
    lhs -= rhs;
    return lhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator*=(T rhs) noexcept {
    storage *= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator*=(vec rhs) noexcept {
    storage *= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator*(vec lhs, T rhs) noexcept {
    lhs *= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator*(T lhs, vec rhs) noexcept {
    rhs *= lhs;
    return rhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator*(vec lhs, vec rhs) noexcept {
    lhs *= rhs;
    return lhs;
  }

  template<std::integral U>
  NUMERIC_ALWAYS_INLINE vec& operator*=(U rhs) noexcept requires std::floating_point<T> {
    storage *= rhs;
    return *this;
  }
  template<std::integral U>
  NUMERIC_ALWAYS_INLINE vec& operator*=(vec<U, N, Align> rhs) noexcept requires std::floating_point<T> {
    storage *= rhs.storage;
    return *this;
  }
  template<std::integral U>
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator*(vec lhs, U rhs) noexcept requires std::floating_point<T> {
    lhs *= rhs;
    return lhs;
  }
  template<std::integral U>
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator*(U lhs, vec rhs) noexcept requires std::floating_point<T> {
    rhs *= lhs;
    return lhs;
  }
  template<std::integral U>
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator*(vec lhs, vec<U, N, Align> rhs) noexcept requires std::floating_point<T> {
    lhs *= rhs;
    return lhs;
  }
  template<std::integral U>
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator*(vec<U, N, Align> lhs, vec rhs) noexcept requires std::floating_point<T> {
    rhs *= lhs;
    return rhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator/=(T rhs) noexcept {
    storage /= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator/=(vec rhs) noexcept {
    storage /= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator/(vec lhs, T rhs) noexcept {
    lhs /= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator/(vec lhs, vec rhs) noexcept {
    lhs /= rhs;
    return lhs;
  }

  template<std::integral U>
  NUMERIC_ALWAYS_INLINE vec& operator/=(U rhs) noexcept requires std::floating_point<T> {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) / static_cast<T>(rhs));
    }
    return *this;
  }
  template<std::integral U>
  NUMERIC_ALWAYS_INLINE vec& operator/=(vec<U, N, Align> rhs) noexcept requires std::floating_point<T> {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) / static_cast<T>(rhs.at(i)));
    }
    return *this;
  }
  template<std::integral U>
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator/(vec lhs, U rhs) noexcept requires std::floating_point<T> {
    lhs /= rhs;
    return lhs;
  }
  template<std::integral U>
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator/(vec lhs, vec<U, N, Align> rhs) noexcept requires std::floating_point<T> {
    lhs /= rhs;
    return lhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator&=(T rhs) noexcept requires std::integral<T> {
    storage &= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator&=(vec rhs) noexcept {
    storage &= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator&(vec lhs, T rhs) noexcept requires std::integral<T> {
    lhs &= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator&(T lhs, vec rhs) noexcept requires std::integral<T> {
    rhs &= lhs;
    return rhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator&(vec lhs, vec rhs) noexcept {
    lhs &= rhs;
    return lhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator|=(T rhs) noexcept requires std::integral<T> {
    storage |= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator|=(vec rhs) noexcept {
    storage |= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator|(vec lhs, T rhs) noexcept requires std::integral<T> {
    lhs |= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator|(T lhs, vec rhs) noexcept requires std::integral<T> {
    rhs |= lhs;
    return rhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator|(vec lhs, vec rhs) noexcept {
    lhs |= rhs;
    return lhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator^=(T rhs) noexcept requires std::integral<T> {
    storage ^= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator^=(vec rhs) noexcept {
    storage ^= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator^(vec lhs, T rhs) noexcept requires std::integral<T> {
    lhs ^= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator^(T lhs, vec rhs) noexcept requires std::integral<T> {
    rhs ^= lhs;
    return rhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator^(vec lhs, vec rhs) noexcept {
    lhs ^= rhs;
    return lhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator>>=(T rhs) noexcept requires std::integral<T> {
    storage >>= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator>>=(vec rhs) noexcept requires std::integral<T> {
    storage >>= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator>>(vec lhs, T rhs) noexcept requires std::integral<T> {
    lhs >>= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator>>(vec lhs, vec rhs) noexcept requires std::integral<T> {
    lhs >>= rhs;
    return lhs;
  }

  NUMERIC_ALWAYS_INLINE vec& operator<<=(T rhs) noexcept requires std::integral<T> {
    storage <<= storage_type::splat(rhs);
    return *this;
  }
  NUMERIC_ALWAYS_INLINE vec& operator<<=(vec rhs) noexcept requires std::integral<T> {
    storage <<= rhs.storage;
    return *this;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator<<(vec lhs, T rhs) noexcept requires std::integral<T> {
    lhs <<= rhs;
    return lhs;
  }
  [[nodiscard]] friend NUMERIC_ALWAYS_INLINE vec operator<<(vec lhs, vec rhs) noexcept requires std::integral<T> {
    lhs <<= rhs;
    return lhs;
  }

  [[nodiscard]] constexpr vec cross(vec const& rhs) const noexcept requires (N == 3) {
    return {
      storage[1] * rhs.storage[2] - storage[2] * rhs.storage[1],
      storage[2] * rhs.storage[0] - storage[0] * rhs.storage[2],
      storage[0] * rhs.storage[1] - storage[1] * rhs.storage[0]
    };
  }

  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec ceil() const noexcept requires std::floating_point<T> {
    vec ret;
    ret.storage = storage.ceil();
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec floor() const noexcept requires std::floating_point<T> {
    vec ret;
    ret.storage = storage.floor();
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec rint() const noexcept requires std::floating_point<T> {
    vec ret;
    ret.storage = storage.rint();
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE vec trunc() const noexcept requires std::floating_point<T> {
    vec ret;
    ret.storage = storage.trunc();
    return ret;
  }

  template<std::integral U>
  [[nodiscard]] explicit NUMERIC_ALWAYS_INLINE operator vec<U, N, Align>() const noexcept requires std::floating_point<T> {
    vec<U, N, Align> ret;
    ret.storage = static_cast<_vec_storage<U, N, Align>>(storage);
    return ret;
  }

  template<std::floating_point U>
  [[nodiscard]] explicit NUMERIC_ALWAYS_INLINE operator vec<U, N, Align>() const noexcept requires std::signed_integral<T> {
    vec<U, N, Align> ret;
    ret.storage = static_cast<_vec_storage<U, N, Align>>(storage);
    return ret;
  }
};


// --- [ Rounding Cast ] ----------------------------------

namespace cast {

template<std::floating_point F, std::size_t N, std::size_t Align>
[[nodiscard]] NUMERIC_ALWAYS_INLINE auto lceil_cast(vec<F, N, Align> v) noexcept {
  vec<long, N, vec<F, N, Align>::storage_type::long_storage_length> ret;
  ret.storage = v.storage._lceil();
  return ret;
}
template<std::floating_point F, std::size_t N, std::size_t Align>
[[nodiscard]] NUMERIC_ALWAYS_INLINE auto lfloor_cast(vec<F, N, Align> v) noexcept {
  vec<long, N, vec<F, N, Align>::storage_type::long_storage_length> ret;
  ret.storage = v.storage._lfloor();
  return ret;
}
template<std::floating_point F, std::size_t N, std::size_t Align>
[[nodiscard]] NUMERIC_ALWAYS_INLINE auto lrint_cast(vec<F, N, Align> v) noexcept {
  vec<long, N, vec<F, N, Align>::storage_type::long_storage_length> ret;
  ret.storage = v.storage._lrint();
  return ret;
}
template<std::floating_point F, std::size_t N, std::size_t Align>
[[nodiscard]] NUMERIC_ALWAYS_INLINE auto lround_cast(vec<F, N, Align> v) noexcept {
  vec<long, N, vec<F, N, Align>::storage_type::long_storage_length> ret;
  ret.storage = v.storage._lround();
  return ret;
}
template<std::floating_point F, std::size_t N, std::size_t Align>
[[nodiscard]] NUMERIC_ALWAYS_INLINE auto ltrunc_cast(vec<F, N, Align> v) noexcept {
  vec<long, N, vec<F, N, Align>::storage_type::long_storage_length> ret;
  ret.storage = v.storage._ltrunc();
  return ret;
}

}

#ifndef DISABLE_NUMERICS_CAST_USING
using namespace cast;
#endif


// --- [ Aliases ] ----------------------------------------

// - LP16:  Windows 16-bit
// - ILP32: Windows 32-bit (x86/WOW64), Windows on ARM (ARM32), watchOS (S5 to S8 SiP)
// - LLP64: Windows 64-bit (AMD64/Intel 64), Windows on ARM64 (ARM64/ARM64EC)
// - LP64:  iOS, macOS, watchOS (S9 SiP or later), Linux
//
// |           | LP16 | ILP32 | LLP64 | LP64 |
// |-----------|-----:|------:|------:|-----:|
// | short     |   16 |    16 |    16 |   16 |
// | int       |   16 |    32 |    32 |   32 |
// | long      |   32 |    32 |    32 |   64 |
// | long long |  (64)|    64 |    64 |   64 |
// | pointer   |   16 |    32 |    64 |   64 |

namespace aliases {

using int2 = vec<int, 2>;
#if INT_MAX == 0x7fffffff
using int2_fast = vec<int, 2, 16>;  // use 128-bit SIMD
#else
using int2_fast = int2;
#endif
using int3 = vec<int, 3>;
using int4 = vec<int, 4>;

using long2 = vec<long, 2>;
#if LONG_MAX == 0x7fffffff
using long2_fast = vec<long, 2, 16>;  // use 128-bit SIMD
#else
using long2_fast = long2;
#endif
using long3 = vec<long, 3>;
using long4 = vec<long, 4>;

using float2 = vec<float, 2>;
using float2_fast = vec<float, 2, 16>;
using float3 = vec<float, 3>;
using float4 = vec<float, 4>;

}

#ifndef DISABLE_NUMERICS_ALIASES_USING
using namespace aliases;
#endif

}  // namespace roxyg::numeric

#pragma warning(pop)
