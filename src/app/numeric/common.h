#pragma once

#ifdef __INTELLISENSE__
#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cmath>
#include <cstdint>
#endif

#if defined(_MSC_VER)
#  define NUMERIC_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define NUMERIC_ALWAYS_INLINE __attribute__((always_inline)) inline
#else
#  define NUMERIC_ALWAYS_INLINE inline
#endif

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
  static constexpr std::size_t padding_length = storage_length - element_length * N;
  static constexpr std::size_t simd_bits = 0;

  using value_type = _basic_vec_storage<T, N>;
  alignas(Align) value_type val;

  [[nodiscard]] constexpr T x() const noexcept requires (N >= 1) { return val[0]; }
  [[nodiscard]] constexpr T y() const noexcept requires (N >= 2) { return val[1]; }
  [[nodiscard]] constexpr T z() const noexcept requires (N >= 3) { return val[2]; }
  [[nodiscard]] constexpr T w() const noexcept requires (N >= 4) { return val[3]; }
  [[nodiscard]] constexpr T at(std::size_t i) const noexcept {
#if _DEBUG
    assert(i >= 0 && i < N && "index out of range");
#endif
    return val.at(i);
  }

  [[nodiscard]] constexpr _vec_storage<T, 2, Align> xy() const noexcept requires (N >= 2) {
    _vec_storage<T, 2, Align> ret;
    ret.setX(x());
    ret.setY(y());
    return ret;
  }
  [[nodiscard]] constexpr _vec_storage<T, 2, Align> zw() const noexcept requires (N >= 4) {
    _vec_storage<T, 2, Align> ret;
    ret.setX(z());
    ret.setY(w());
    return ret;
  }

  constexpr void setX(T rhs) noexcept requires (N >= 1) { val[0] = rhs; }
  constexpr void setY(T rhs) noexcept requires (N >= 2) { val[1] = rhs; }
  constexpr void setZ(T rhs) noexcept requires (N >= 3) { val[2] = rhs; }
  constexpr void setW(T rhs) noexcept requires (N >= 4) { val[3] = rhs; }
  constexpr void setAt(std::size_t i, T rhs) noexcept {
#if _DEBUG
    assert(i >= 0 && i < N && "index out of range");
#endif
    val.at(i) = rhs;
  }

  constexpr void setXY(_vec_storage<T, 2, Align> rhs) noexcept requires (N >= 2) {
    setAt(0, rhs.at(0));
    setAt(1, rhs.at(1));
  }
  constexpr void setZW(_vec_storage<T, 2, Align> rhs) noexcept requires (N >= 4) {
    setAt(2, rhs.at(0));
    setAt(3, rhs.at(1));
  }

  [[nodiscard]] constexpr static _vec_storage splat(T s) noexcept {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, s);
    }
    return ret;
  }
  [[nodiscard]] constexpr static _vec_storage make(std::initializer_list<T> v) noexcept {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, v[i]);
    }
    return ret;
  }
  [[nodiscard]] constexpr static _vec_storage concat(std::initializer_list<_vec_storage<T, 2, 16>> v) noexcept requires (N % 2 == 0) {
    _vec_storage ret;
    for (std::size_t i = 0; i < (N >> 1); ++i) {
      ret.setAt(2 * i, v[i].x());
      ret.setAt(2 * i + 1, v[i].y());
    }
    return ret;
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

  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage ceil() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::ceil(at(i)));
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage floor() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::floor(at(i)));
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage rint() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::rint(at(i)));
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage trunc() const noexcept requires std::floating_point<T> {
    _vec_storage ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::trunc(at(i)));
    }
    return ret;
  }

  static constexpr std::size_t long_storage_length = __storage_size<long, _vec_storage<T, N, Align>>();
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lceil() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, static_cast<long>(std::ceil(at(i))));
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lfloor() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, static_cast<long>(std::floor(at(i))));
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lrint() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::lrint(at(i)));
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _lround() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, std::lround(at(i)));
    }
    return ret;
  }
  [[nodiscard]] NUMERIC_ALWAYS_INLINE _vec_storage<long, N, long_storage_length> _ltrunc() const noexcept requires std::floating_point<T> {
    _vec_storage<long, N, long_storage_length> ret;
    for (std::size_t i = 0; i < N; ++i) {
      ret.setAt(i, static_cast<long>(at(i)));
    }
    return ret;
  }
};

}  // namespace roxyg::numeric
