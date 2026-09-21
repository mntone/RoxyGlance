#include "pch.h"
#include "app/utility/backoff/jitter.h"

namespace test::roxyg::utility {

template<typename Generator, uint32_t MinVal>
struct JitterGeneratorCase {
  using GeneratorType = Generator;
  static constexpr uint32_t min_value{MinVal};
};

using JitterGeneratorCases = ::testing::Types<
  JitterGeneratorCase<::roxyg::utility::xorshift32_equal_jitter_generator, 50>,
  JitterGeneratorCase<::roxyg::utility::xorshift32_full_jitter_generator, 0>,
  JitterGeneratorCase<::roxyg::utility::xorshift64_equal_jitter_generator, 50>,
  JitterGeneratorCase<::roxyg::utility::xorshift64_full_jitter_generator, 0>
>;

template<typename Case>
class JitterGeneratorTest: public ::testing::Test {
public:
  using Generator = typename Case::GeneratorType;
};

class JitterGeneratorNames final {
public:
  template<typename T>
  static std::string GetName(int) {
    using G = typename T::GeneratorType;

    if constexpr (std::is_same_v<G, ::roxyg::utility::xorshift32_equal_jitter_generator>) {
      return "xorshift32_equal";
    }
    if constexpr (std::is_same_v<G, ::roxyg::utility::xorshift32_full_jitter_generator>) {
      return "xorshift32_full";
    }
    if constexpr (std::is_same_v<G, ::roxyg::utility::xorshift64_equal_jitter_generator>) {
      return "xorshift64_equal";
    }
    if constexpr (std::is_same_v<G, ::roxyg::utility::xorshift64_full_jitter_generator>) {
      return "xorshift64_full";
    }
  }
};

TYPED_TEST_SUITE(JitterGeneratorTest, JitterGeneratorCases, JitterGeneratorNames);

TYPED_TEST(JitterGeneratorTest, GeneratesValuesWithinRange) {
  typename TestFixture::Generator jitter;

  for (int index = 0; index < 100; ++index) {
    uint32_t const value = jitter.random(100u);
    EXPECT_GE(value, TypeParam::min_value);
    EXPECT_LE(value, 100u);
  }
}

TEST(JitterGeneratorTest, DisabledJitterReturnsInput) {
  ::roxyg::utility::no_jitter_generator jitter;

  EXPECT_EQ(jitter.random(100u), 100u);
}

}  // namespace test::roxyg::utility
