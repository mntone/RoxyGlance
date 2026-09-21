#include "pch.h"
#include "app/utility/random.h"

namespace test::roxyg::utility {

TEST(Xorshift32, ProducesReproducibleSequence) {
  ::roxyg::utility::xorshift32 first{12345u};
  ::roxyg::utility::xorshift32 second{12345u};

  for (int index = 0; index < 100; ++index) {
    EXPECT_EQ(first(), second());
  }
}

TEST(Xorshift32, ZeroSeedUsesDefaultSequence) {
  ::roxyg::utility::xorshift32 zero_seed{0u};
  ::roxyg::utility::xorshift32 default_seed;

  EXPECT_EQ(zero_seed(), default_seed());
}

TEST(Xorshift32, ProducesValuesWithinRange) {
  ::roxyg::utility::xorshift32 generator{12345u};

  for (int index = 0; index < 100; ++index) {
    uint32_t const value = generator();
    EXPECT_GE(value, generator.min());
    EXPECT_LE(value, generator.max());
  }
}

TEST(Xorshift64, ProducesReproducibleSequence) {
  ::roxyg::utility::xorshift64 first{12345u};
  ::roxyg::utility::xorshift64 second{12345u};

  for (int index = 0; index < 100; ++index) {
    EXPECT_EQ(first(), second());
  }
}

TEST(Xorshift64, ZeroSeedUsesDefaultSequence) {
  ::roxyg::utility::xorshift64 zero_seed{0u};
  ::roxyg::utility::xorshift64 default_seed;

  EXPECT_EQ(zero_seed(), default_seed());
}

TEST(Xorshift64, ProducesValuesWithinRange) {
  ::roxyg::utility::xorshift64 generator{12345u};

  for (int index = 0; index < 100; ++index) {
    uint64_t const value = generator();
    EXPECT_GE(value, generator.min());
    EXPECT_LE(value, generator.max());
  }
}

TEST(MakeSeed, ProducesReproducibleSeeds) {
  EXPECT_EQ(
    ::roxyg::utility::make_seed32(123456789u, 42u),
    ::roxyg::utility::make_seed32(123456789u, 42u));
  EXPECT_EQ(
    ::roxyg::utility::make_seed64(123456789u, 42u),
    ::roxyg::utility::make_seed64(123456789u, 42u));
}

TEST(MakeSeed, IncorporatesThreadId) {
  EXPECT_NE(
    ::roxyg::utility::make_seed32(123456789u, 1u),
    ::roxyg::utility::make_seed32(123456789u, 2u));
  EXPECT_NE(
    ::roxyg::utility::make_seed64(123456789u, 1u),
    ::roxyg::utility::make_seed64(123456789u, 2u));
}

TEST(MakeSeed, IncorporatesTimestampCounter) {
  EXPECT_NE(
    ::roxyg::utility::make_seed32(1u, 42u),
    ::roxyg::utility::make_seed32(2u, 42u));
  EXPECT_NE(
    ::roxyg::utility::make_seed64(1u, 42u),
    ::roxyg::utility::make_seed64(2u, 42u));
}

}  // namespace test::roxyg::utility
