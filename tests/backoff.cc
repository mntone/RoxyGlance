#include "pch.h"
#include "app/utility/backoff/fixed.h"
#include "app/utility/backoff/exponential.h"

namespace test::roxyg::utility {

using ::roxyg::utility::detail::scale_and_clamp;

template<float Scale, uint32_t Expected>
struct ScaleAndClampCase final {
  using ScalerType = scale_and_clamp<uint32_t, 1000, Scale>;

  static constexpr uint32_t expected{Expected};
};

using ScaleAndClampCases = ::testing::Types<
  ScaleAndClampCase<1.125f, 112>,
  ScaleAndClampCase<1.2f, 120>,
  ScaleAndClampCase<1.25f, 125>,
  ScaleAndClampCase<1.333333f, 133>,
  ScaleAndClampCase<1.375f, 137>,
  ScaleAndClampCase<1.5f, 150>,
  ScaleAndClampCase<1.625f, 162>,
  ScaleAndClampCase<1.75f, 175>,
  ScaleAndClampCase<1.875f, 187>,
  ScaleAndClampCase<2.f, 200>,
  ScaleAndClampCase<2.5f, 250>,
  ScaleAndClampCase<3.f, 300>,
  ScaleAndClampCase<3.5f, 350>,
  ScaleAndClampCase<4.f, 400>
>;

template<typename Case>
class ScaleAndClampTest: public ::testing::Test {
public:
  using Scalar = Case::ScalerType;
};

class ScaleAndClampNames final {
public:
  template<typename T>
  static std::string GetName(int) {
    using S = typename T::ScalerType;

    int const scale = static_cast<int>(S::scale * 1000.f);
    return std::format("{:4d}", scale);
  }
};

TYPED_TEST_SUITE(ScaleAndClampTest, ScaleAndClampCases, ScaleAndClampNames);

TYPED_TEST(ScaleAndClampTest, ScalesValue) {
  typename TestFixture::Scalar scale;

  EXPECT_EQ(scale(100u), TypeParam::expected);
}

TEST(ScaleAndClampTest, ClampsScaledValue) {
  scale_and_clamp<uint32_t, 1000, 2.f> scale;

  EXPECT_EQ(scale(600u), 1000u);
}
TEST(FixedBackoff, ReturnsConfiguredDelay) {
  ::roxyg::utility::uint32_fixed_backoff<1000> backoff;

  EXPECT_EQ(backoff.current(), 1000u);
  EXPECT_EQ(backoff.next(), 1000u);
}

TEST(FixedBackoff, ResetPreservesConfiguredDelay) {
  ::roxyg::utility::uint32_fixed_backoff<1000> backoff;

  backoff.reset();

  EXPECT_EQ(backoff.current(), 1000u);
  EXPECT_EQ(backoff.next(), 1000u);
}

TEST(ExponentialBackoff, IncreasesWithoutJitter) {
  ::roxyg::utility::uint32_exponential_backoff<100, 800, 2.f> backoff;

  EXPECT_EQ(backoff.current(), 100u);
  EXPECT_EQ(backoff.next(), 200u);
  EXPECT_EQ(backoff.next(), 400u);
  EXPECT_EQ(backoff.next(), 800u);
}

TEST(ExponentialBackoff, ClampsAtMaximumDelay) {
  ::roxyg::utility::uint32_exponential_backoff<100, 800, 2.f> backoff;

  EXPECT_EQ(backoff.next(), 200u);
  EXPECT_EQ(backoff.next(), 400u);
  EXPECT_EQ(backoff.next(), 800u);
  EXPECT_EQ(backoff.next(), 800u);
}

TEST(ExponentialBackoff, ResetRestoresMinimumDelay) {
  ::roxyg::utility::uint32_exponential_backoff<100, 800, 2.f> backoff;

  EXPECT_EQ(backoff.next(), 200u);
  backoff.reset();

  EXPECT_EQ(backoff.current(), 100u);
  EXPECT_EQ(backoff.next(), 200u);
}

}  // namespace test::roxyg::utility


