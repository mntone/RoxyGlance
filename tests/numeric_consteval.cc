#include "pch.h"
#include "app/numeric/numeric.h"
#include "numeric_shared.h"

namespace test::roxyg::numeric {

using ConstevalVectorTestTypes = ::testing::Types<
  TypedVectorConfig<int, 2>,
#if INT_MAX == 0x7fffffff
  TypedVectorConfig<int, 2, 16>,
#endif
  TypedVectorConfig<int, 3>,
  TypedVectorConfig<int, 4>,
  TypedVectorConfig<long, 2>,
#if LONG_MAX == 0x7fffffff
  TypedVectorConfig<long, 2, 16>,
#endif
  TypedVectorConfig<long, 3>,
  TypedVectorConfig<long, 4>,
  TypedVectorConfig<float, 2>,
  TypedVectorConfig<float, 2, 16>,
  TypedVectorConfig<float, 3>,
  TypedVectorConfig<float, 4>
>;

template<typename Config>
class ConstevalTypedVectorTest: public ::testing::Test {
public:
  using T = typename Config::T;
  static constexpr std::size_t N = Config::N;
  static constexpr std::size_t A = Config::A;
  using V = vec<T, N, A>;

protected:
  static constexpr std::array<short, 17> kTestData = {
    2,  3, 4,  5,
    4, -5, 3, -6,
    1,  3, 2,  5,
    6,  3, 4,  5,
    3,
  };
};
TYPED_TEST_SUITE(ConstevalTypedVectorTest, ConstevalVectorTestTypes, TypedVectorNameGenerator);

TYPED_TEST(ConstevalTypedVectorTest, GetAccessors) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;
  constexpr std::size_t A = TestFixture::A;

  if constexpr (std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) == 4 && A == 16) {
    constexpr V c = {.storage = {.val = {.m128i_i32 = {1, 2, 3, 4}}}};
    if constexpr (N >= 1 && N <= 4) {
      static_assert(c.x() == 1, "x() failed");
      static_assert(c.at(0) == 1, "at(0) failed");
      if constexpr (N >= 2) {
        static_assert(c.y() == 2, "y() failed");
        static_assert(c.at(1) == 2, "at(1) failed");
        static_assert(c.xy().x() == 1, "xy().x() failed");
        static_assert(c.xy().y() == 2, "xy().y() failed");
        if constexpr (N >= 3) {
          static_assert(c.z() == 3, "z() failed");
          static_assert(c.at(2) == 3, "at(2) failed");
          if constexpr (N == 4) {
            static_assert(c.w() == 4, "w() failed");
            static_assert(c.at(3) == 4, "at(3) failed");
            static_assert(c.zw().x() == 3, "zw().x() failed");
            static_assert(c.zw().y() == 4, "zw().y() failed");
          }
        }
      }
    } else {
      static_assert(false, "Compile-time is not supported");
    }
  } else if constexpr (std::is_floating_point_v<T> && sizeof(T) == 4 && A == 16) {
    constexpr V c = {.storage = {.val = {.m128_f32 = {1.f, 2.f, 3.f, 4.f}}}};
    if constexpr (N >= 1 && N <= 4) {
      static_assert(c.x() == 1.f, "x() failed");
      static_assert(c.at(0) == 1.f, "at(0) failed");
      if constexpr (N >= 2) {
        static_assert(c.y() == 2.f, "y() failed");
        static_assert(c.at(1) == 2.f, "at(1) failed");
        static_assert(c.xy().x() == 1.f, "xy().x() failed");
        static_assert(c.xy().y() == 2.f, "xy().y() failed");
        if constexpr (N >= 3) {
          static_assert(c.z() == 3.f, "z() failed");
          static_assert(c.at(2) == 3.f, "at(2) failed");
          if constexpr (N == 4) {
            static_assert(c.w() == 4.f, "w() failed");
            static_assert(c.at(3) == 4.f, "at(3) failed");
            static_assert(c.zw().x() == 3.f, "zw().x() failed");
            static_assert(c.zw().y() == 4.f, "zw().y() failed");
          }
        }
      }
    } else {
      static_assert(false, "Compile-time is not supported");
    }
  } else {
    if constexpr (N >= 4) {
      constexpr V c = {.storage = {.val = {{1, 2, 3, 4}}}};
      static_assert(c.x() == 1, "x() failed");
      static_assert(c.y() == 2, "y() failed");
      static_assert(c.z() == 3, "z() failed");
      static_assert(c.w() == 4, "w() failed");
      static_assert(c.at(0) == 1, "at(0) failed");
      static_assert(c.at(1) == 2, "at(1) failed");
      static_assert(c.at(2) == 3, "at(2) failed");
      static_assert(c.at(3) == 4, "at(3) failed");
      static_assert(c.xy().x() == 1, "xy().x() failed");
      static_assert(c.xy().y() == 2, "xy().y() failed");
      static_assert(c.zw().x() == 3, "zw().x() failed");
      static_assert(c.zw().y() == 4, "zw().y() failed");
    } else if constexpr (N >= 3) {
      constexpr V c = {.storage = {.val = {{1, 2, 3}}}};
      static_assert(c.x() == 1, "x() failed");
      static_assert(c.y() == 2, "y() failed");
      static_assert(c.z() == 3, "z() failed");
      static_assert(c.at(0) == 1, "at(0) failed");
      static_assert(c.at(1) == 2, "at(1) failed");
      static_assert(c.at(2) == 3, "at(2) failed");
      static_assert(c.xy().x() == 1, "xy().x() failed");
      static_assert(c.xy().y() == 2, "xy().y() failed");
    } else if constexpr (N >= 2) {
      constexpr V c = {.storage = {.val = {{1, 2}}}};
      static_assert(c.x() == 1, "x() failed");
      static_assert(c.y() == 2, "y() failed");
      static_assert(c.at(0) == 1, "at(0) failed");
      static_assert(c.at(1) == 2, "at(1) failed");
      static_assert(c.xy().x() == 1, "xy().x() failed");
      static_assert(c.xy().y() == 2, "xy().y() failed");
    } else if constexpr (N >= 1) {
      constexpr V c = {.storage = {.val = {{1}}}};
      static_assert(c.x() == 1, "x() failed");
      static_assert(c.at(0) == 1, "at(0) failed");
    } else {
      static_assert(false, "Compile-time is not supported");
    }
  }
}

TYPED_TEST(ConstevalTypedVectorTest, BuildFunctions) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;
  constexpr std::size_t A = TestFixture::A;

  // vec::splat
  constexpr V b = V::splat(9);
  if constexpr (N >= 1) {
    static_assert(b.x() == 9, "splat failed at x");
    if constexpr (N >= 2) {
      static_assert(b.y() == 9, "splat failed at y");
      if constexpr (N >= 3) {
        static_assert(b.z() == 9, "splat failed at z");
        if constexpr (N >= 4) {
          static_assert(b.w() == 9, "splat failed at w");
        }
      } else if constexpr (V::storage_type::simd_bits != 0) {
        static_assert(b.storage.w() == 0, "splat failed at w");
      }
    } else if constexpr (V::storage_type::simd_bits != 0) {
      static_assert(b.storage.z() == 0, "splat failed at z");
      static_assert(b.storage.w() == 0, "splat failed at w");
    }
  } else if constexpr (V::storage_type::simd_bits != 0) {
    static_assert(b.storage.y() == 0, "splat failed at y");
    static_assert(b.storage.z() == 0, "splat failed at z");
    static_assert(b.storage.w() == 0, "splat failed at w");
  }

  // vec::make
  if constexpr (N == 1) {
    constexpr V c = V::make(1);
    static_assert(c.x() == 1, "make failed at x");
    if constexpr (V::storage_type::simd_bits != 0) {
      static_assert(c.storage.y() == 0, "make failed at y");
      static_assert(c.storage.z() == 0, "make failed at z");
      static_assert(c.storage.w() == 0, "make failed at w");
    }
  } else if constexpr (N == 2) {
    constexpr V c = V::make(1, 2);
    static_assert(c.x() == 1, "make failed at x");
    static_assert(c.y() == 2, "make failed at y");
    if constexpr (V::storage_type::simd_bits != 0) {
      static_assert(c.storage.z() == 0, "make failed at z");
      static_assert(c.storage.w() == 0, "make failed at w");
    }
  } else if constexpr (N == 3) {
    constexpr V c = V::make(1, 2, 3);
    static_assert(c.x() == 1, "make failed at x");
    static_assert(c.y() == 2, "make failed at y");
    static_assert(c.z() == 3, "make failed at z");
    if constexpr (V::storage_type::simd_bits != 0) {
      static_assert(c.storage.w() == 0, "make failed at w");
    }
  } else if constexpr (N == 4) {
    constexpr V c = V::make(1, 2, 3, 4);
    static_assert(c.x() == 1, "make failed at x");
    static_assert(c.y() == 2, "make failed at y");
    static_assert(c.z() == 3, "make failed at z");
    static_assert(c.w() == 4, "make failed at w");
  } else {
    static_assert(false, "Compile-time is not supported");
  }

  // vec::concat
  if constexpr (V::storage_type::simd_bits != 0 && N == 4) {
    using V2 = vec<T, 2, A>;
    constexpr V2 xy = V2::make(2, 4);
    constexpr V2 zw = V2::make(6, 3);
    constexpr V m = V::concat(xy, zw);
    static_assert(m.x() == 2, "concat failed at x");
    static_assert(m.y() == 4, "concat failed at y");
    static_assert(m.z() == 6, "concat failed at z");
    static_assert(m.w() == 3, "concat failed at w");
  }
}

TYPED_TEST(ConstevalTypedVectorTest, CompareOperators) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;

  if constexpr (N == 1) {
    constexpr V a = V::make(1);
    constexpr V b = V::make(-1);
    static_assert(a == a, "a == a failed");
    static_assert(!(a == b), "!(a == b) failed");
    static_assert(a != b, "a != b failed");
    static_assert(!(a != a), "!(a != a) failed");
  } else if constexpr (N == 2) {
    constexpr V a = V::make(1, 2);
    constexpr V b = V::make(-1, 2);
    static_assert(a == a, "a == a failed");
    static_assert(!(a == b), "!(a == b) failed");
    static_assert(a != b, "a != b failed");
    static_assert(!(a != a), "!(a != a) failed");
  } else if constexpr (N == 3) {
    constexpr V a = V::make(1, 2, 3);
    constexpr V b = V::make(-1, 2, 3);
    static_assert(a == a, "a == a failed");
    static_assert(!(a == b), "!(a == b) failed");
    static_assert(a != b, "a != b failed");
    static_assert(!(a != a), "!(a != a) failed");
  } else if constexpr (N == 4) {
    constexpr V a = V::make(1, 2, 3, 4);
    constexpr V b = V::make(-1, 2, 3, 4);
    static_assert(a == a, "a == a failed");
    static_assert(!(a == b), "!(a == b) failed");
    static_assert(a != b, "a != b failed");
    static_assert(!(a != a), "!(a != a) failed");
  } else {
    static_assert(false, "Compile-time is not supported");
  }
}

TYPED_TEST(ConstevalTypedVectorTest, UnaryOperators) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;

  if constexpr (N == 1) {
    constexpr V c = V::make(1);
    static_assert(+c == V::make(1), "+c failed");
    static_assert(-c == V::make(-1), "-c failed");

    constexpr V c_inv = ~c;
    static_assert(c_inv.x() == __logical_bit_not<T>(1), "~c failed at x()");
  } else if constexpr (N == 2) {
    constexpr V c = V::make(1, 2);
    static_assert(+c == V::make(1, 2), "+c failed");
    static_assert(-c == V::make(-1, -2), "-c failed");

    constexpr V c_inv = ~c;
    static_assert(c_inv.x() == __logical_bit_not<T>(1), "~c failed at x()");
    static_assert(c_inv.y() == __logical_bit_not<T>(2), "~c failed at y()");
  } else if constexpr (N == 3) {
    constexpr V c = V::make(1, 2, 3);
    static_assert(+c == V::make(1, 2, 3), "+c failed");
    static_assert(-c == V::make(-1, -2, -3), "-c failed");

    constexpr V c_inv = ~c;
    static_assert(c_inv.x() == __logical_bit_not<T>(1), "~c failed at x()");
    static_assert(c_inv.y() == __logical_bit_not<T>(2), "~c failed at y()");
    static_assert(c_inv.z() == __logical_bit_not<T>(3), "~c failed at z()");
  } else if constexpr (N == 4) {
    constexpr V c = V::make(1, 2, 3, 4);
    static_assert(+c == V::make(1, 2, 3, 4), "+c failed");
    static_assert(-c == V::make(-1, -2, -3, -4), "-c failed");

    constexpr V c_inv = ~c;
    static_assert(c_inv.x() == __logical_bit_not<T>(1), "~c failed at x()");
    static_assert(c_inv.y() == __logical_bit_not<T>(2), "~c failed at y()");
    static_assert(c_inv.z() == __logical_bit_not<T>(3), "~c failed at z()");
    static_assert(c_inv.w() == __logical_bit_not<T>(4), "~c failed at w()");
  } else {
    static_assert(false, "Compile-time is not supported");
  }
}

TYPED_TEST(ConstevalTypedVectorTest, VectorScalarOperators) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;

  constexpr T s = -2;
  constexpr T c = 2;
  if constexpr (N == 1) {
    constexpr V a = V::make(4);
    static_assert((a + s) == V::make(2), "a + s failed");
    static_assert((s + a) == V::make(2), "s + a failed");
    static_assert((a - s) == V::make(6), "a - s failed");
    static_assert((a * s) == V::make(-8), "a * s failed");
    static_assert((s * a) == V::make(-8), "s * a failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert((a / c) == V::make(2), "a / c failed");
      static_assert((a & s) == V::make(2), "a & b failed");
      static_assert((a | s) == V::make(-4), "a | b failed");
      static_assert((a ^ s) == V::make(__logical_bit_xor(4.f, -2.f)), "a ^ b failed");
    } else {
      static_assert((a & s) == V::make(4), "a & s failed");
      static_assert((a | s) == V::make(-2), "a | s failed");
      static_assert((a ^ s) == V::make(-6), "a ^ s failed");
      static_assert((a << c) == V::make(16), "a << c failed");
      static_assert((a >> c) == V::make(1), "a >> c failed");
    }
  } else if constexpr (N == 2) {
    constexpr V a = V::make(4, 8);
    static_assert((a + s) == V::make(2, 6), "a + s failed");
    static_assert((s + a) == V::make(2, 6), "s + a failed");
    static_assert((a - s) == V::make(6, 10), "a - s failed");
    static_assert((a * s) == V::make(-8, -16), "a * s failed");
    static_assert((s * a) == V::make(-8, -16), "s * a failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert((a / c) == V::make(2, 4), "a / c failed");
      static_assert((a & s) == V::make(2, 2), "a & b failed");
      static_assert((a | s) == V::make(-4, -8), "a | b failed");
      static_assert((a ^ s) == V::make(__logical_bit_xor(4.f, -2.f), __logical_bit_xor(8.f, -2.f)), "a ^ b failed");
    } else {
      static_assert((a & s) == V::make(4, 8), "a & s failed");
      static_assert((a | s) == V::make(-2, -2), "a | s failed");
      static_assert((a ^ s) == V::make(-6, -10), "a ^ s failed");
      static_assert((a << c) == V::make(16, 32), "a << c failed");
      static_assert((a >> c) == V::make(1, 2), "a >> c failed");
    }
  } else if constexpr (N == 3) {
    constexpr V a = V::make(4, 8, 1);
    static_assert((a + s) == V::make(2, 6, -1), "a + s failed");
    static_assert((s + a) == V::make(2, 6, -1), "s + a failed");
    static_assert((a - s) == V::make(6, 10, 3), "a - s failed");
    static_assert((a * s) == V::make(-8, -16, -2), "a * s failed");
    static_assert((s * a) == V::make(-8, -16, -2), "s * a failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert((a / c) == V::make(2, 4, static_cast<T>(0.5)), "a / c failed");
      static_assert((a & s) == V::make(2, 2, 0), "a & b failed");
      static_assert((a | s) == V::make(-4, -8, -std::numeric_limits<T>::infinity()), "a | b failed");
      static_assert((a ^ s) == V::make(__logical_bit_xor(4.f, -2.f), __logical_bit_xor(8.f, -2.f), -std::numeric_limits<T>::infinity()), "a ^ b failed");
    } else {
      static_assert((a & s) == V::make(4, 8, 0), "a & s failed");
      static_assert((a | s) == V::make(-2, -2, -1), "a | s failed");
      static_assert((a ^ s) == V::make(-6, -10, -1), "a ^ s failed");
      static_assert((a << c) == V::make(16, 32, 4), "a << c failed");
      static_assert((a >> c) == V::make(1, 2, 0), "a >> c failed");
    }
  } else if constexpr (N == 4) {
    constexpr V a = V::make(4, 8, 1, 0);
    static_assert((a + s) == V::make(2, 6, -1, -2), "a + s failed");
    static_assert((s + a) == V::make(2, 6, -1, -2), "s + a failed");
    static_assert((a - s) == V::make(6, 10, 3, 2), "a - s failed");
    static_assert((a * s) == V::make(-8, -16, -2, -0), "a * s failed");
    static_assert((s * a) == V::make(-8, -16, -2, -0), "s * a failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert((a / c) == V::make(2, 4, static_cast<T>(0.5), 0), "a / c failed");
      static_assert((a & s) == V::make(2, 2, 0, 0), "a & b failed");
      static_assert((a | s) == V::make(-4, -8, -std::numeric_limits<T>::infinity(), -2), "a | b failed");
      static_assert((a ^ s) == V::make(__logical_bit_xor(4.f, -2.f), __logical_bit_xor(8.f, -2.f), -std::numeric_limits<T>::infinity(), -2), "a ^ b failed");
    } else {
      static_assert((a & s) == V::make(4, 8, 0, 0), "a & s failed");
      static_assert((a | s) == V::make(-2, -2, -1, -2), "a | s failed");
      static_assert((a ^ s) == V::make(-6, -10, -1, -2), "a ^ s failed");
      static_assert((a << c) == V::make(16, 32, 4, 0), "a << c failed");
      static_assert((a >> c) == V::make(1, 2, 0, 0), "a >> c failed");
    }
  } else {
    static_assert(false, "Compile-time is not supported");
  }
}

TYPED_TEST(ConstevalTypedVectorTest, VectorVectorOperators) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;

  if constexpr (N == 1) {
    constexpr V a = V::make(4);
    constexpr V b = V::make(-2);
    constexpr V c = V::make(0);
    static_assert(a + b == V::make(2), "a + b failed");
    static_assert(a - b == V::make(6), "a - b failed");
    static_assert(a * b == V::make(-8), "a * b failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert(a / b == V::make(-2), "a / b failed");
      static_assert((a & b) == V::make(2), "a & b failed");
      static_assert((a | b) == V::make(-4), "a | b failed");
      static_assert((a ^ b) == V::make(__logical_bit_xor(4.f, -2.f)), "a ^ b failed");
    } else {
      static_assert((a & b) == V::make(4), "a & b failed");
      static_assert((a | b) == V::make(-2), "a | b failed");
      static_assert((a ^ b) == V::make(-6), "a ^ b failed");
      static_assert((a << c) == V::make(4), "a << c failed");
      static_assert((a >> c) == V::make(4), "a >> c failed");
    }
  } else if constexpr (N == 2) {
    constexpr V a = V::make(4, 8);
    constexpr V b = V::make(-2, 4);
    constexpr V c = V::make(0, 1);
    static_assert(a + b == V::make(2, 12), "a + b failed");
    static_assert(a - b == V::make(6, 4), "a - b failed");
    static_assert(a * b == V::make(-8, 32), "a * b failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert(a / b == V::make(-2, 2), "a / b failed");
      static_assert((a & b) == V::make(2, 2), "a & b failed");
      static_assert((a | b) == V::make(-4, 16), "a | b failed");
      static_assert((a ^ b) == V::make(__logical_bit_xor(4.f, -2.f), __logical_bit_xor(8.f, 4.f)), "a ^ b failed");
    } else {
      static_assert((a & b) == V::make(4, 0), "a & b failed");
      static_assert((a | b) == V::make(-2, 12), "a | b failed");
      static_assert((a ^ b) == V::make(-6, 12), "a ^ b failed");
      static_assert((a << c) == V::make(4, 16), "a << c failed");
      static_assert((a >> c) == V::make(4, 4), "a >> c failed");
    }
  } else if constexpr (N == 3) {
    constexpr V a = V::make(4, 8, 1);
    constexpr V b = V::make(-2, 4, -1);
    constexpr V c = V::make(0, 1, 0);
    static_assert(a + b == V::make(2, 12, 0), "a + b failed");
    static_assert(a - b == V::make(6, 4, 2), "a - b failed");
    static_assert(a * b == V::make(-8, 32, -1), "a * b failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert(a / b == V::make(-2, 2, -1), "a / b failed");
      static_assert((a & b) == V::make(2, 2, 1), "a & b failed");
      static_assert((a | b) == V::make(-4, 16, -1), "a | b failed");
      static_assert((a ^ b) == V::make(__logical_bit_xor(4.f, -2.f), __logical_bit_xor(8.f, 4.f), -0), "a ^ b failed");
    } else {
      static_assert((a & b) == V::make(4, 0, 1), "a & b failed");
      static_assert((a | b) == V::make(-2, 12, -1), "a | b failed");
      static_assert((a ^ b) == V::make(-6, 12, -2), "a ^ b failed");
      static_assert((a << c) == V::make(4, 16, 1), "a << c failed");
      static_assert((a >> c) == V::make(4, 4, 1), "a >> c failed");
    }
  } else if constexpr (N == 4) {
    constexpr V a = V::make(4, 8, 1, 0);
    constexpr V b = V::make(-2, 4, -1, 2);
    constexpr V c = V::make(0, 1, 0, 3);
    static_assert(a + b == V::make(2, 12, 0, 2), "a + b failed");
    static_assert(a - b == V::make(6, 4, 2, -2), "a - b failed");
    static_assert(a * b == V::make(-8, 32, -1, 0), "a * b failed");
    if constexpr (std::is_floating_point_v<T>) {
      static_assert(a / b == V::make(-2, 2, -1, 0), "a / b failed");
      static_assert((a & b) == V::make(2, 2, 1, 0), "a & b failed");
      static_assert((a | b) == V::make(-4, 16, -1, 2), "a | b failed");
      static_assert((a ^ b) == V::make(__logical_bit_xor(4.f, -2.f), __logical_bit_xor(8.f, 4.f), -0, 2), "a ^ b failed");
    } else {
      static_assert((a & b) == V::make(4, 0, 1, 0), "a & b failed");
      static_assert((a | b) == V::make(-2, 12, -1, 2), "a | b failed");
      static_assert((a ^ b) == V::make(-6, 12, -2, 2), "a ^ b failed");
      static_assert((a << c) == V::make(4, 16, 1, 0), "a << c failed");
      static_assert((a >> c) == V::make(4, 4, 1, 0), "a >> c failed");
    }
  } else {
    static_assert(false, "Compile-time is not supported");
  }
}

}  // namespace test::roxyg::numeric
