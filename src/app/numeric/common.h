#pragma once

#ifdef __INTELLISENSE__
#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cmath>
#include <cstdint>
#include <type_traits>
#endif

#if defined(_MSC_VER)
#  define NUMERIC_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define NUMERIC_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#  define NUMERIC_ALWAYS_INLINE inline
#endif

#if defined(__cpp_if_consteval) && __cpp_if_consteval >= 202106L
#  define NUMERIC_CONSTEXPR_IF_SUPPORTED constexpr
#  define NUMERIC_IF_CONSTEVAL_          if consteval
#elif defined(__cpp_lib_is_constant_evaluated) && __cpp_lib_is_constant_evaluated >= 201811L
#  define NUMERIC_CONSTEXPR_IF_SUPPORTED constexpr
#  define NUMERIC_IF_CONSTEVAL_          if (std::is_constant_evaluated())
#elif defined(__GNUC__)
#  define NUMERIC_CONSTEXPR_IF_SUPPORTED constexpr
#  define NUMERIC_IF_CONSTEVAL_          if (__builtin_is_constant_evaluated())
#else
#  define NUMERIC_CONSTEXPR_IF_SUPPORTED
#  define NUMERIC_IF_CONSTEVAL_          if (0)
#endif

#define NUMERIC_INLINE_CONSTEXPR NUMERIC_ALWAYS_INLINE NUMERIC_CONSTEXPR_IF_SUPPORTED

namespace roxyg::numeric {

template<typename N>
concept arithmetic = std::integral<N> || std::floating_point<N>;

constexpr std::size_t __alignup(std::size_t x, std::size_t align) noexcept {
  return (x + (align - 1)) & ~(align - 1);
}

template<typename T, typename S>
constexpr std::size_t __storage_size() noexcept {
  return S::storage_length / S::element_length * sizeof(T);
}

template<typename T>
using _bitop_t
= std::conditional_t<std::is_integral<T>::value, T,
  std::conditional_t<sizeof(T) == 1, uint8_t,
  std::conditional_t<sizeof(T) == 2, uint16_t,
  std::conditional_t<sizeof(T) == 4, uint32_t,
  std::conditional_t<sizeof(T) == 8, uint64_t, void>>>>>;

template<arithmetic T, std::size_t N, std::size_t Align>
struct __simd_vec_tags {
  static constexpr std::size_t element_count = N;
  static constexpr std::size_t element_length = sizeof(T);
  static constexpr std::size_t storage_length = Align;
  static constexpr std::size_t storage_count = storage_length / element_length;
  static constexpr std::size_t padding_length = storage_length - element_length * element_count;
  static constexpr std::size_t simd_bits = storage_length * 8;
};

template<typename T, std::size_t N>
using _basic_vec_storage = std::array<T, N>;

template<arithmetic T, std::size_t N, std::size_t Align = std::bit_ceil(sizeof(T)* N)>
  requires (N >= 1)
struct _vec_storage {
  static constexpr std::size_t element_count = N;
  static constexpr std::size_t element_length = sizeof(T);
  static constexpr std::size_t storage_length = Align;
  static constexpr std::size_t storage_count = storage_length / element_length;
  static constexpr std::size_t padding_length = storage_length - element_length * N;
  static constexpr std::size_t simd_bits = 0;

  using value_type = _basic_vec_storage<T, N>;
  alignas(Align) value_type val;

  NUMERIC_ALWAYS_INLINE constexpr T x() const noexcept { return val[0]; }
  NUMERIC_ALWAYS_INLINE constexpr T y() const noexcept { return val[1]; }
  NUMERIC_ALWAYS_INLINE constexpr T z() const noexcept { return val[2]; }
  NUMERIC_ALWAYS_INLINE constexpr T w() const noexcept { return val[3]; }
  NUMERIC_ALWAYS_INLINE constexpr T at(std::size_t i) const noexcept {
#if _DEBUG
    assert(i >= 0 && i < N && "index out of range");
#endif
    return val.at(i);
  }

  NUMERIC_ALWAYS_INLINE constexpr _vec_storage<T, 2, Align> xy() const noexcept {
    return {val[0], val[1]};
  }
  NUMERIC_ALWAYS_INLINE constexpr _vec_storage<T, 2, Align> zw() const noexcept {
    return {val[2], val[3]};
  }

  NUMERIC_ALWAYS_INLINE constexpr void setX(T rhs) noexcept { val[0] = rhs; }
  NUMERIC_ALWAYS_INLINE constexpr void setY(T rhs) noexcept { val[1] = rhs; }
  NUMERIC_ALWAYS_INLINE constexpr void setZ(T rhs) noexcept { val[2] = rhs; }
  NUMERIC_ALWAYS_INLINE constexpr void setW(T rhs) noexcept { val[3] = rhs; }
  NUMERIC_ALWAYS_INLINE constexpr void setAt(std::size_t i, T rhs) noexcept {
#if _DEBUG
    assert(i >= 0 && i < N && "index out of range");
#endif
    val[i] = rhs;
  }

  NUMERIC_ALWAYS_INLINE constexpr void setXY(_vec_storage<T, 2, Align> rhs) noexcept {
    val[0] = rhs.val[0];
    val[1] = rhs.val[1];
  }
  NUMERIC_ALWAYS_INLINE constexpr void setZW(_vec_storage<T, 2, Align> rhs) noexcept {
    val[2] = rhs.val[0];
    val[3] = rhs.val[1];
  }

  static NUMERIC_ALWAYS_INLINE constexpr _vec_storage splat(T s) noexcept {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.val[i] = s;
    }
    return ret;
  }
  static NUMERIC_ALWAYS_INLINE constexpr _vec_storage make(T x, T y, T z, T w) noexcept {
    return {x, y, z, w};
  }
  static NUMERIC_ALWAYS_INLINE constexpr _vec_storage make(T x, T y, T z) noexcept {
    return {x, y, z};
  }
  static NUMERIC_ALWAYS_INLINE constexpr _vec_storage make(T x, T y) noexcept {
    return {x, y};
  }
  static NUMERIC_ALWAYS_INLINE constexpr _vec_storage make(T x) noexcept {
    return {x};
  }
  static NUMERIC_ALWAYS_INLINE constexpr _vec_storage concat(_vec_storage<T, 2, Align> xy, _vec_storage<T, 2, Align> zw) noexcept {
    return {xy.val[0], xy.val[1], zw.val[0], zw.val[1]};
  }

  friend constexpr bool operator==(_vec_storage lhs, _vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      if (lhs.at(i) != rhs.at(i)) {
        return false;
      }
    }
    return true;
  }
  friend constexpr bool operator!=(_vec_storage lhs, _vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      if (lhs.at(i) != rhs.at(i)) {
        return true;
      }
    }
    return false;
  }

  constexpr _vec_storage operator-() const noexcept requires (std::signed_integral<T> || std::floating_point<T>) {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, -at(i));
    }
    return ret;
  }
  constexpr _vec_storage operator~() const noexcept {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::bit_cast<T>(~std::bit_cast<_bitop_t<T>>(at(i))));
    }
    return ret;
  }

  constexpr _vec_storage& operator+=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) + rhs.at(i));
    }
    return *this;
  }
  constexpr _vec_storage& operator-=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) - rhs.at(i));
    }
    return *this;
  }
  constexpr _vec_storage& operator*=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) * rhs.at(i));
    }
    return *this;
  }
  constexpr _vec_storage& operator/=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) / rhs.at(i));
    }
    return *this;
  }

  constexpr _vec_storage& operator&=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, std::bit_cast<T>(std::bit_cast<_bitop_t<T>>(at(i)) & std::bit_cast<_bitop_t<T>>(rhs.at(i))));
    }
    return *this;
  }
  constexpr _vec_storage& operator|=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, std::bit_cast<T>(std::bit_cast<_bitop_t<T>>(at(i)) | std::bit_cast<_bitop_t<T>>(rhs.at(i))));
    }
    return *this;
  }
  constexpr _vec_storage& operator^=(_vec_storage rhs) noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, std::bit_cast<T>(std::bit_cast<_bitop_t<T>>(at(i)) ^ std::bit_cast<_bitop_t<T>>(rhs.at(i))));
    }
    return *this;
  }

  constexpr _vec_storage& operator>>=(_vec_storage rhs) noexcept requires std::integral<T> {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) >> rhs.at(i));
    }
    return *this;
  }
  constexpr _vec_storage& operator<<=(_vec_storage rhs) noexcept requires std::integral<T> {
    for (std::size_t i = 0; i < N; ++i) {
      setAt(i, at(i) << rhs.at(i));
    }
    return *this;
  }

  NUMERIC_ALWAYS_INLINE _vec_storage ceil() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::ceil(at(i)));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage floor() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::floor(at(i)));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage rint() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::rint(at(i)));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage trunc() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::trunc(at(i)));
    }
    return ret;
  }

  static constexpr std::size_t long_storage_length = __storage_size<long, _vec_storage<T, N, Align>>();
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lceil() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, static_cast<long>(std::ceil(at(i))));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lfloor() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, static_cast<long>(std::floor(at(i))));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lrint() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::lrint(at(i)));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lround() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::lround(at(i)));
    }
    return ret;
  }
  NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _ltrunc() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, static_cast<long>(at(i)));
    }
    return ret;
  }
};

}  // namespace roxyg::numeric
