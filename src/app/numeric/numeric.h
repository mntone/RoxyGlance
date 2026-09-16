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

  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR T x() const noexcept requires (N >= 1) { return storage.x(); }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR T y() const noexcept requires (N >= 2) { return storage.y(); }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR T z() const noexcept requires (N >= 3) { return storage.z(); }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR T w() const noexcept requires (N >= 4) { return storage.w(); }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR T at(std::size_t i) const noexcept { return storage.at(i); }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR T operator[](std::size_t i) const { return storage.at(i); }

  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR vec<T, 2, Align> xy() const noexcept requires (N >= 2) { return {storage.xy()}; }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR vec<T, 2, Align> zw() const noexcept requires (N >= 4) { return {storage.zw()}; }

  NUMERIC_INLINE_CONSTEXPR void setX(T rhs) noexcept requires (N >= 1) { storage.setX(rhs); }
  NUMERIC_INLINE_CONSTEXPR void setY(T rhs) noexcept requires (N >= 2) { storage.setY(rhs); }
  NUMERIC_INLINE_CONSTEXPR void setZ(T rhs) noexcept requires (N >= 3) { storage.setZ(rhs); }
  NUMERIC_INLINE_CONSTEXPR void setW(T rhs) noexcept requires (N >= 4) { storage.setW(rhs); }
  NUMERIC_INLINE_CONSTEXPR void setAt(std::size_t i, T rhs) noexcept { storage.setAt(i, rhs); }

  NUMERIC_INLINE_CONSTEXPR void setXY(vec<T, 2, Align> rhs) noexcept requires (N >= 2) {
    NUMERIC_IF_CONSTEVAL_{
      storage.setX(rhs.x());
      storage.setY(rhs.y());
    } else {
      storage.setXY(rhs.storage);
    }
  }
  NUMERIC_INLINE_CONSTEXPR void setZW(vec<T, 2, Align> rhs) noexcept requires (N >= 4) {
    NUMERIC_IF_CONSTEVAL_{
      storage.setZ(rhs.x());
      storage.setW(rhs.y());
    } else {
      storage.setZW(rhs.storage);
    }
  }

  [[nodiscard]] static NUMERIC_INLINE_CONSTEXPR vec splat(T s) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      vec ret;
      for (std::size_t i = 0; i < storage_type::storage_count; ++i) {
        ret.storage.setAt(i, i < N ? s : 0);
      }
      return ret;
    } else {
      return vec{storage_type::splat(s)};
    }
  }
  [[nodiscard]] static NUMERIC_INLINE_CONSTEXPR vec make(T x, T y, T z, T w) noexcept requires (N == 4) {
    return vec{storage_type::make(x, y, z, w)};
  }
  [[nodiscard]] static NUMERIC_INLINE_CONSTEXPR vec make(T x, T y, T z) noexcept requires (N == 3) {
    return vec{storage_type::make(x, y, z)};
  }
  [[nodiscard]] static NUMERIC_INLINE_CONSTEXPR vec make(T x, T y) noexcept requires (N == 2) {
    return vec{storage_type::make(x, y)};
  }
  [[nodiscard]] static NUMERIC_INLINE_CONSTEXPR vec make(T x) noexcept requires (N == 1) {
    return vec{storage_type::make(x)};
  }
  [[nodiscard]] static NUMERIC_INLINE_CONSTEXPR vec<T, 4> concat(vec<T, 2, 16> xy, vec<T, 2, 16> zw) noexcept requires (N == 4) {
    NUMERIC_IF_CONSTEVAL_{
      return vec{storage_type::make(xy.x(), xy.y(), zw.x(), zw.y())};
    } else {
      return vec{storage_type::concat(xy.storage, zw.storage)};
    }
  }

  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR bool operator==(vec lhs, vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        if (lhs.storage.at(i) != rhs.storage.at(i)) {
          return false;
        }
      }
      return true;
    } else {
      return lhs.storage == rhs.storage;
    }
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR bool operator!=(vec lhs, vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        if (lhs.storage.at(i) != rhs.storage.at(i)) {
          return true;
        }
      }
      return false;
    } else {
      return lhs.storage != rhs.storage;
    }
  }

  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR vec operator+() const noexcept {
    return *this;
  }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR vec operator-() const noexcept requires (std::signed_integral<T> || std::floating_point<T>) {
    NUMERIC_IF_CONSTEVAL_{
      vec ret;
      for (std::size_t i = 0; i < storage_type::storage_count; ++i) {
        ret.storage.setAt(i, i < N ? -storage.at(i) : 0);
      }
      return ret;
    } else {
      return vec{-storage};
    }
  }
  [[nodiscard]] NUMERIC_INLINE_CONSTEXPR vec operator~() const noexcept {
    NUMERIC_IF_CONSTEVAL_{
      vec ret;
      for (std::size_t i = 0; i < storage_type::storage_count; ++i) {
        ret.storage.setAt(i, i < N ? __logical_bit_not(storage.at(i)) : 0);
      }
      return ret;
    } else {
      return vec{~storage};
    }
  }

  NUMERIC_INLINE_CONSTEXPR vec& operator+=(vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, storage.at(i) + rhs.storage.at(i));
      }
    } else {
      storage += rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator+=(T rhs) noexcept { return *this += splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator+(vec lhs, vec rhs) noexcept {
    return lhs += rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator+(vec lhs, T rhs) noexcept {
    return lhs += rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator+(T lhs, vec rhs) noexcept {
    return rhs += lhs;
  }

  NUMERIC_INLINE_CONSTEXPR vec& operator-=(vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, storage.at(i) - rhs.storage.at(i));
      }
    } else {
      storage -= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator-=(T rhs) noexcept { return *this -= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator-(vec lhs, vec rhs) noexcept {
    return lhs -= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator-(vec lhs, T rhs) noexcept {
    return lhs -= rhs;
  }

  NUMERIC_INLINE_CONSTEXPR vec& operator*=(vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, storage.at(i) * rhs.storage.at(i));
      }
    } else {
      storage *= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator*=(T rhs) noexcept { return *this *= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator*(vec lhs, vec rhs) noexcept {
    return lhs *= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator*(vec lhs, T rhs) noexcept {
    return lhs *= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator*(T lhs, vec rhs) noexcept {
    return rhs *= lhs;
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

  NUMERIC_INLINE_CONSTEXPR vec& operator/=(vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, storage.at(i) / rhs.storage.at(i));
      }
    } else {
      storage /= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator/=(T rhs) noexcept { return *this /= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator/(vec lhs, vec rhs) noexcept {
    return lhs /= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator/(vec lhs, T rhs) noexcept {
    return lhs /= rhs;
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

  NUMERIC_INLINE_CONSTEXPR vec& operator&=(vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, __logical_bit_and(storage.at(i), rhs.storage.at(i)));
      }
    } else {
      storage &= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator&=(T rhs) noexcept { return *this &= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator&(vec lhs, vec rhs) noexcept {
    return lhs &= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator&(vec lhs, T rhs) noexcept {
    return lhs &= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator&(T lhs, vec rhs) noexcept {
    return rhs &= lhs;
  }

  NUMERIC_INLINE_CONSTEXPR vec& operator|=(vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, __logical_bit_or(storage.at(i), rhs.storage.at(i)));
      }
    } else {
      storage |= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator|=(T rhs) noexcept { return *this |= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator|(vec lhs, vec rhs) noexcept {
    return lhs |= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator|(vec lhs, T rhs) noexcept {
    return lhs |= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator|(T lhs, vec rhs) noexcept {
    return rhs |= lhs;
  }

  NUMERIC_INLINE_CONSTEXPR vec& operator^=(vec rhs) noexcept {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, __logical_bit_xor(storage.at(i), rhs.storage.at(i)));
      }
    } else {
      storage ^= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator^=(T rhs) noexcept { return *this ^= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator^(vec lhs, vec rhs) noexcept {
    return lhs ^= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator^(vec lhs, T rhs) noexcept {
    return lhs ^= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator^(T lhs, vec rhs) noexcept {
    return rhs ^= lhs;
  }

  NUMERIC_INLINE_CONSTEXPR vec& operator>>=(vec rhs) noexcept requires std::integral<T> {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, storage.at(i) >> rhs.storage.at(i));
      }
    } else {
      storage >>= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator>>=(T rhs) noexcept requires std::integral<T> { return *this >>= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator>>(vec lhs, vec rhs) noexcept requires std::integral<T> {
    return lhs >>= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator>>(vec lhs, T rhs) noexcept requires std::integral<T> {
    return lhs >>= rhs;
  }

  NUMERIC_INLINE_CONSTEXPR vec& operator<<=(vec rhs) noexcept requires std::integral<T> {
    NUMERIC_IF_CONSTEVAL_{
      for (std::size_t i = 0; i < N; ++i) {
        storage.setAt(i, storage.at(i) << rhs.storage.at(i));
      }
    } else {
      storage <<= rhs.storage;
    }
    return *this;
  }
  NUMERIC_INLINE_CONSTEXPR vec& operator<<=(T rhs) noexcept requires std::integral<T> { return *this <<= splat(rhs); }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator<<(vec lhs, vec rhs) noexcept requires std::integral<T> {
    return lhs <<= rhs;
  }
  [[nodiscard]] friend NUMERIC_INLINE_CONSTEXPR vec operator<<(vec lhs, T rhs) noexcept requires std::integral<T> {
    return lhs <<= rhs;
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

#ifndef DISABLE_NUMERIC_CAST_USING
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

#ifndef DISABLE_NUMERIC_ALIASES_USING
using namespace aliases;
#endif

}  // namespace roxyg::numeric

#pragma warning(pop)
