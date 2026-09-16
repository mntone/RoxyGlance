#include "pch.h"
#include "app/numeric/numeric.h"
#include "numeric_shared.h"

namespace test::roxyg::numeric {

using VectorTestTypes = ::testing::Types<
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
class TypedVectorTest: public ::testing::Test {
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

  static V makeVector(std::size_t offset) noexcept {
    V v;
    for (std::size_t i = 0; i < N; ++i) {
      v.setAt(i, static_cast<T>(kTestData[(offset + i) % kTestData.size()]));
    }
    return v;
  }

  static T makeScalar(std::size_t offset) noexcept {
    return static_cast<T>(kTestData[offset % kTestData.size()]);
  }

  void expectSingleGetter(
    std::string_view target,
    V const& input,
    T(*const accessor_operation)(V const&),
    std::size_t index,
    std::size_t offset = 0
  ) {
    EXPECT_EQ(accessor_operation(input), makeScalar(offset + index));
  }

  void expectSingleSetter(
    std::string_view target,
    V actual,
    void (*const accessor_operation)(V&),
    T expected,
    std::size_t index
  ) {
    accessor_operation(actual);
    EXPECT_EQ(actual.at(index), expected);
  }

  void expectCompareOperation(
    std::string_view operation,
    V const& lhs,
    V const& rhs,
    bool(* const vector_operation)(V const&, V const&),
    bool expected
  ) {
    EXPECT_EQ(vector_operation(lhs, rhs), expected);
  }

  void expectUnaryOperation(
    std::string_view operation,
    V const& input,
    V(* const vector_operation)(V const&),
    T(* const scalar_operation)(T)
  ) {
    V const actual = vector_operation(input);
    for (std::size_t i = 0; i < N; ++i) {
      SCOPED_TRACE(::testing::Message()
        << operation << " at index " << i
        << " (input: " << input[i] << ")");
      EXPECT_EQ(actual[i], scalar_operation(input[i]));
    }
  }

  void expectVectorScalarOperation(
    std::string_view operation,
    V const& input,
    T scalar,
    V(* const vector_operation)(V const&, T),
    T(* const scalar_operation)(T, T)
  ) {
    V const actual = vector_operation(input, scalar);
    for (std::size_t i = 0; i < N; ++i) {
      SCOPED_TRACE(::testing::Message()
        << operation << " at index " << i
        << " (input: " << input[i] << ", scalar: " << scalar << ")");
      EXPECT_EQ(actual[i], scalar_operation(input[i], scalar));
    }
  }

  void expectVectorVectorOperation(
    std::string_view operation,
    V const& lhs,
    V const& rhs,
    V(* const vector_operation)(V const&, V const&),
    T(* const scalar_operation)(T, T)
  ) {
    V const actual = vector_operation(lhs, rhs);
    for (std::size_t i = 0; i < N; ++i) {
      SCOPED_TRACE(::testing::Message()
        << operation << " at index " << i
        << " (lhs: " << lhs[i] << ", rhs: " << rhs[i] << ")");
      EXPECT_EQ(actual[i], scalar_operation(lhs[i], rhs[i]));
    }
  }
};
TYPED_TEST_SUITE(TypedVectorTest, VectorTestTypes, TypedVectorNameGenerator);

TYPED_TEST(TypedVectorTest, GetAccessors) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;

  auto const input = this->makeVector(0);

  this->expectSingleGetter("x", input, [](V const& a) { return a.x(); }, 0);
  if constexpr (N >= 2) {
    this->expectSingleGetter("y", input, [](V const& a) { return a.y(); }, 1);
    if constexpr (N >= 3) {
      this->expectSingleGetter("z", input, [](V const& a) { return a.z(); }, 2);
      if constexpr (N >= 4) {
        this->expectSingleGetter("w", input, [](V const& a) { return a.w(); }, 3);
      }
    }
  }
}

TYPED_TEST(TypedVectorTest, SetAccessors) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;

  auto const input = this->makeVector(0);

  this->expectSingleSetter("x", input, [](V& a) { a.setX(24); }, 24, 0);
  if constexpr (N >= 2) {
    this->expectSingleSetter("y", input, [](V& a) { a.setY(32); }, 32, 1);
    if constexpr (N >= 3) {
      this->expectSingleSetter("z", input, [](V& a) { a.setZ(27); }, 27, 2);
      if constexpr (N >= 4) {
        this->expectSingleSetter("w", input, [](V& a) { a.setW(47); }, 47, 3);
      }
    }
  }
}

TYPED_TEST(TypedVectorTest, BuildFunctions) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;
  constexpr std::size_t N = TestFixture::N;
  constexpr std::size_t A = TestFixture::A;
  using V4 = ::roxyg::numeric::vec<T, 4>;

  if constexpr (V::storage_type::simd_bits != 0 && A == V4::storage_type::storage_length) {
    // vec::splat
    V4 const actual = V4{.storage = {.val = V::splat(2).storage.val}};
    V4 expected;
    if constexpr (N == 1) {
      expected = V4::make(2, 0, 0, 0);
    } else if constexpr (N == 2) {
      expected = V4::make(2, 2, 0, 0);
    } else if constexpr (N == 3) {
      expected = V4::make(2, 2, 2, 0);
    } else if constexpr (N == 4) {
      expected = V4::make(2, 2, 2, 2);
    }
    EXPECT_EQ(actual, expected);

    // vec::make
    if constexpr (N == 1) {
      V4 const actual = V4{.storage = {.val = V::make(6).storage.val}};
      EXPECT_EQ(actual, V4::make(6, 0, 0, 0));
    } else if constexpr (N == 2) {
      V4 const actual = V4{.storage = {.val = V::make(6, 7).storage.val}};
      EXPECT_EQ(actual, V4::make(6, 7, 0, 0));
    } else if constexpr (N == 3) {
      V4 const actual = V4{.storage = {.val = V::make(6, 7, 8).storage.val}};
      EXPECT_EQ(actual, V4::make(6, 7, 8, 0));
    } else if constexpr (N == 4) {
      V4 const actual = V4{.storage = {.val = V::make(6, 7, 8, 9).storage.val}};
      EXPECT_EQ(actual, V4::make(6, 7, 8, 9));
    }

    // vec::concat
    if constexpr (N == 4) {
      using V2 = ::roxyg::numeric::vec<T, 2, A>;
      V const actual = V::concat(V2::make(11, 12), V2::make(13, 14));
      EXPECT_EQ(actual, V4::make(11, 12, 13, 14));
    }
  }
}

TYPED_TEST(TypedVectorTest, CompareOperators) {
  using V = typename TestFixture::V;

  auto const lhs = this->makeVector(0);
  auto const rhs = this->makeVector(12);
  this->expectCompareOperation(
    "vector == vector (true)", lhs, lhs,
    [](V const& a, V const& b) { return a == b; },
    true
  );
  this->expectCompareOperation(
    "vector == vector (false)", lhs, rhs,
    [](V const& a, V const& b) { return a == b; },
    false
  );
  this->expectCompareOperation(
    "vector != vector (true)", lhs, rhs,
    [](V const& a, V const& b) { return a != b; },
    true
  );
  this->expectCompareOperation(
    "vector != vector (false)", lhs, lhs,
    [](V const& a, V const& b) { return a != b; },
    false
  );
}

TYPED_TEST(TypedVectorTest, UnaryOperators) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;

  auto const input = this->makeVector(0);

  this->expectUnaryOperation(
    "unary +", input,
    [](V const& a) { return +a; },
    [](T a) { return +a; }
  );
  this->expectUnaryOperation(
    "unary -", input,
    [](V const& a) { return -a; },
    [](T a) { return -a; }
  );
  this->expectUnaryOperation(
    "unary ~", input,
    [](V const& a) { return ~a; },
    [](T a) { return std::bit_cast<T>(~std::bit_cast<_bitop_t<T>>(a)); }
  );
}

TYPED_TEST(TypedVectorTest, VectorScalarOperators) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;

  auto const input = this->makeVector(0);
  auto const scalar = this->makeScalar(16);

  this->expectVectorScalarOperation(
    "vector + scalar", input, scalar,
    [](V const& a, T s) { return a + s; },
    [](T a, T b) { return a + b; }
  );
  this->expectVectorScalarOperation(
    "vector - scalar", input, scalar,
    [](V const& a, T s) { return a - s; },
    [](T a, T b) { return a - b; }
  );
  this->expectVectorScalarOperation(
    "vector * scalar", input, scalar,
    [](V const& a, T s) { return a * s; },
    [](T a, T b) { return a * b; }
  );
  if constexpr (std::is_floating_point_v<T>) {
    this->expectVectorScalarOperation(
      "vector / scalar", input, scalar,
      [](V const& a, T s) { return a / s; },
      [](T a, T b) { return a / b; }
    );
  }

  if constexpr (std::integral<T>) {
    this->expectVectorScalarOperation(
      "vector & scalar", input, scalar,
      [](V const& a, T s) { return a & s; },
      [](T a, T b) { return a & b; }
    );
    this->expectVectorScalarOperation(
      "vector | scalar", input, scalar,
      [](V const& a, T s) { return a | s; },
      [](T a, T b) { return a | b; }
    );
    this->expectVectorScalarOperation(
      "vector ^ scalar", input, scalar,
      [](V const& a, T s) { return a ^ s; },
      [](T a, T b) { return a ^ b; }
    );

    this->expectVectorScalarOperation(
      "vector >> scalar", input, scalar,
      [](V const& a, T s) { return a >> s; },
      [](T a, T b) { return a >> b; }
    );
    this->expectVectorScalarOperation(
      "vector << scalar", input, scalar,
      [](V const& a, T s) { return a << s; },
      [](T a, T b) { return a << b; }
    );
  }
}

TYPED_TEST(TypedVectorTest, VectorVectorOperators) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;

  auto const lhs = this->makeVector(0);
  auto const rhs = this->makeVector(4);
  auto const rhs_pos = this->makeVector(8);

  this->expectVectorVectorOperation(
    "vector + vector", lhs, rhs,
    [](V const& a, V const& b) { return a + b; },
    [](T a, T b) { return a + b; }
  );
  this->expectVectorVectorOperation(
    "vector - vector", lhs, rhs,
    [](V const& a, V const& b) { return a - b; },
    [](T a, T b) { return a - b; }
  );
  this->expectVectorVectorOperation(
    "vector * vector",
    lhs, rhs,
    [](V const& a, V const& b) { return a * b; },
    [](T a, T b) { return a * b; }
  );
  if constexpr (std::is_floating_point_v<T>) {
    this->expectVectorVectorOperation(
      "vector / vector", lhs, rhs,
      [](V const& a, V const& b) { return a / b; },
      [](T a, T b) { return a / b; }
    );
    this->expectVectorVectorOperation(
      "vector / vector(+)", lhs, rhs_pos,
      [](V const& a, V const& b) { return a / b; },
      [](T a, T b) { return a / b; }
    );
  }

  this->expectVectorVectorOperation(
    "vector & vector", lhs, rhs,
    [](V const& a, V const& b) { return a & b; },
    [](T a, T b) { return std::bit_cast<T>(std::bit_cast<_bitop_t<T>>(a) & std::bit_cast<_bitop_t<T>>(b)); }
  );
  this->expectVectorVectorOperation(
    "vector | vector", lhs, rhs,
    [](V const& a, V const& b) { return a | b; },
    [](T a, T b) { return std::bit_cast<T>(std::bit_cast<_bitop_t<T>>(a) | std::bit_cast<_bitop_t<T>>(b)); }
  );
  this->expectVectorVectorOperation(
    "vector ^ vector", lhs, rhs,
    [](V const& a, V const& b) { return a ^ b; },
    [](T a, T b) { return std::bit_cast<T>(std::bit_cast<_bitop_t<T>>(a) ^ std::bit_cast<_bitop_t<T>>(b)); }
  );

  if constexpr (std::integral<T>) {
    this->expectVectorVectorOperation(
      "vector >> vector", lhs, rhs_pos,
      [](V const& a, V const& b) { return a >> b; },
      [](T a, T b) { return a >> b; }
    );
    this->expectVectorVectorOperation(
      "vector << vector", lhs, rhs_pos,
      [](V const& a, V const& b) { return a << b; },
      [](T a, T b) { return a << b; }
    );
  }
}

}  // namespace test::roxyg::numeric
