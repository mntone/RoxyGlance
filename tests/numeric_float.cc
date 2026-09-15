#include <string_view>
#include "pch.h"
#include "app/numeric/numeric.h"
#include "numeric_shared.h"

namespace test::roxyg::numeric {

using FloatVectorTestTypes = ::testing::Types<
  TypedVectorConfig<float, 2>,
  TypedVectorConfig<float, 2, 16>,
  TypedVectorConfig<float, 3>,
  TypedVectorConfig<float, 4>
>;

template<typename Config>
class TypedFloatVectorTest: public ::testing::Test {
public:
  using T = typename Config::T;
  static constexpr std::size_t N = Config::N;
  static constexpr std::size_t A = Config::A;
  using V = vec<T, N, A>;

protected:
  static constexpr std::array<double, 16> kTestData = {
    -2.9, -2.5, -2.1, -0.5,
    -0.1,  0.0,  0.1,  0.4,
     0.5,  0.9,  1.0,  1.5,
     2.1,  2.5,  2.9,  3.8
  };

  static V makeVector(std::size_t offset) {
    V v;
    for (std::size_t i = 0; i < N; ++i) {
      v.setAt(i, static_cast<T>(kTestData[(offset + i) % kTestData.size()]));
    }
    return v;
  }

  template<typename OutElem>
  void expectConvertOperation(
    std::string_view operation,
    vec<OutElem, N, A> (*const vector_operation)(V const&),
    OutElem (*const scalar_operation)(T)
  ) {
    constexpr std::size_t chunk_count = (kTestData.size() + N - 1) / N;
    for (std::size_t chunk = 0; chunk < chunk_count; ++chunk) {
      auto const input = makeVector(chunk * N);
      vec<OutElem, N, A> const actual = vector_operation(input);

      for (std::size_t i = 0; i < N; ++i) {
        SCOPED_TRACE(::testing::Message()
          << operation << " at chunk " << chunk
          << ", index " << i
          << " (input: " << input[i] << ")");
        EXPECT_EQ(actual[i], static_cast<OutElem>(scalar_operation(input[i])));
      }
    }
  }
};
TYPED_TEST_SUITE(TypedFloatVectorTest, FloatVectorTestTypes, TypedVectorNameGenerator);

TYPED_TEST(TypedFloatVectorTest, RoundingOperations) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;

  this->expectConvertOperation<float>(
    "ceil",
    [](V const& v) { return v.ceil(); },
    [](T value) { return std::ceil(value); }
  );
  this->expectConvertOperation<float>(
    "floor",
    [](V const& v) { return v.floor(); },
    [](T value) { return std::floor(value); }
  );
  this->expectConvertOperation<float>(
    "rint",
    [](V const& v) { return v.rint(); },
    [](T value) { return std::rint(value); }
  );
  this->expectConvertOperation<float>(
    "trunc",
    [](V const& v) { return v.trunc(); },
    [](T value) { return std::trunc(value); }
  );
}

TYPED_TEST(TypedFloatVectorTest, IntegerCastOperations) {
  using T = typename TestFixture::T;
  using V = typename TestFixture::V;

  this->expectConvertOperation<long>(
    "lceil_cast",
    [](V const& v) { return lceil_cast(v); },
    [](T value) { return static_cast<long>(std::ceil(value)); }
  );
  this->expectConvertOperation<long>(
    "lfloor_cast",
    [](V const& v) { return lfloor_cast(v); },
    [](T value) { return static_cast<long>(std::floor(value)); }
  );
  this->expectConvertOperation<long>(
    "lrint_cast",
    [](V const& v) { return lrint_cast(v); },
    [](T value) { return std::lrint(value); }
  );
  this->expectConvertOperation<long>(
    "lround_cast",
    [](V const& v) { return lround_cast(v); },
    [](T value) { return std::lround(value); }
  );
  this->expectConvertOperation<long>(
    "ltrunc_cast",
    [](V const& v) { return ltrunc_cast(v); },
    [](T value) { return static_cast<long>(value); }
  );
}

}  // namespace test::roxyg::numeric
