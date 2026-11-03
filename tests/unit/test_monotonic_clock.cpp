// TC-SENS-009 (ST-005): elapsed_us and TimestampUs comparison operators.
// Verifies: FR-011, NFR-001 (structural)

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <utility>

#include "dfcs/fault_code.hpp"
#include "dfcs/monotonic_clock.hpp"
#include "dfcs/status.hpp"
#include "fake_clock.hpp"

namespace {

using dfcs::core::elapsed_us;
using dfcs::core::FaultCode;
using dfcs::core::MonotonicClock;
using dfcs::core::StatusCode;
using dfcs::core::TimestampUs;
using dfcs::test::FakeClock;

constexpr std::uint64_t kMax = std::numeric_limits<std::uint64_t>::max();

}  // namespace

// TC-SENS-009.1
TEST(ElapsedUs, LaterAfterEarlierReturnsDifference) {
  constexpr TimestampUs kEarlier{2000};
  constexpr TimestampUs kLater{2750};

  const auto r = elapsed_us(kLater, kEarlier);

  ASSERT_TRUE(r.is_ok());
  EXPECT_EQ(r.value(), kLater.value - kEarlier.value);
}

// TC-SENS-009.2
TEST(ElapsedUs, EqualReturnsZero) {
  constexpr TimestampUs kT{2000};

  const auto r = elapsed_us(kT, kT);

  ASSERT_TRUE(r.is_ok());
  EXPECT_EQ(r.value(), 0U);
}

// TC-SENS-009.3 (symbolic base is below the maximum uint64 value)
TEST(ElapsedUs, OneTickAfterReturnsOne) {
  constexpr TimestampUs kEarlier{2000};
  constexpr TimestampUs kLater{kEarlier.value + 1};

  const auto r = elapsed_us(kLater, kEarlier);

  ASSERT_TRUE(r.is_ok());
  EXPECT_EQ(r.value(), 1U);
}

// TC-SENS-009.4 (symbolic base is below the maximum uint64 value)
TEST(ElapsedUs, OneTickBeforeRejected) {
  constexpr TimestampUs kLater{2000};
  constexpr TimestampUs kEarlier{kLater.value + 1};

  const auto r = elapsed_us(kLater, kEarlier);

  EXPECT_FALSE(r.is_ok());
  EXPECT_EQ(r.status().fault, FaultCode::TIME_NON_MONOTONIC);
}

// TC-SENS-009.5
TEST(ElapsedUs, MaximumDifference) {
  const auto r = elapsed_us(TimestampUs{kMax}, TimestampUs{0});

  ASSERT_TRUE(r.is_ok());
  EXPECT_EQ(r.value(), kMax);
}

// TC-SENS-009.6 (backward-clock injection)
TEST(ElapsedUs, BackwardReturnsTimeNonMonotonic) {
  constexpr TimestampUs kLater{1000};
  constexpr TimestampUs kEarlier{5000};

  const auto r = elapsed_us(kLater, kEarlier);

  EXPECT_FALSE(r.is_ok());
  EXPECT_EQ(r.status().code, StatusCode::InvalidArgument);
  EXPECT_EQ(r.status().fault, FaultCode::TIME_NON_MONOTONIC);
}

// TC-SENS-009.7 (a wrapped difference would be 1; a failed result must not look fresh)
TEST(ElapsedUs, BackwardIsNeitherZeroNorWrapped) {
  constexpr std::uint64_t kSentinel = 0xDEADBEEFU;

  const auto r = elapsed_us(TimestampUs{0}, TimestampUs{kMax});

  EXPECT_FALSE(r.is_ok());
  EXPECT_EQ(r.value_or(kSentinel), kSentinel);
}

// TC-SENS-009.9
TEST(ElapsedUs, MatchesFakeClockAdvance) {
  constexpr std::uint64_t kD = 4321;
  FakeClock clock{TimestampUs{1000}};
  const TimestampUs start = clock.now();

  ASSERT_TRUE(clock.advance(kD).is_ok());
  const auto r = elapsed_us(clock.now(), start);

  ASSERT_TRUE(r.is_ok());
  EXPECT_EQ(r.value(), kD);
}

// TC-SENS-009.12 (NFR-001 structural evidence)
TEST(ElapsedUs, ClockContractIsNoexcept) {
  static_assert(noexcept(elapsed_us(std::declval<TimestampUs>(), std::declval<TimestampUs>())));
  static_assert(noexcept(std::declval<const MonotonicClock&>().now()));
}

// TC-SENS-009.15
TEST(TimestampUs, ComparisonOrdersDistinctValues) {
  constexpr TimestampUs a{100};
  constexpr TimestampUs b{200};

  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_FALSE(a >= b);
}

// TC-SENS-009.16
TEST(TimestampUs, ComparisonEqualValues) {
  constexpr TimestampUs a{100};
  constexpr TimestampUs b{100};

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_FALSE(a < b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_TRUE(a >= b);
}
