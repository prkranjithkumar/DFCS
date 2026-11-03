// TC-SENS-011 (ST-005): FakeClock behavior (construction, advance, set, interface, noexcept).
// Also TC-SENS-009.10/.11/.13/.14: FakeClock advance boundaries and overflow injection.
// Verifies: FR-011, NFR-001 (structural)

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include "dfcs/fault_code.hpp"
#include "dfcs/monotonic_clock.hpp"
#include "dfcs/status.hpp"
#include "fake_clock.hpp"

namespace {

using dfcs::core::FaultCode;
using dfcs::core::MonotonicClock;
using dfcs::core::Status;
using dfcs::core::StatusCode;
using dfcs::core::TimestampUs;
using dfcs::test::FakeClock;

constexpr std::uint64_t kMax = std::numeric_limits<std::uint64_t>::max();
constexpr TimestampUs kT0{1000};

void ExpectStatus(const Status& s, StatusCode code, FaultCode fault) {
  EXPECT_EQ(s.code, code);
  EXPECT_EQ(s.fault, fault);
  EXPECT_EQ(s.is_ok(), code == StatusCode::Ok);
}

}  // namespace

// TC-SENS-011.1
TEST(FakeClock, ReturnsConstructionTime) {
  const FakeClock clock{kT0};
  EXPECT_EQ(clock.now().value, kT0.value);
}

// TC-SENS-011.2
TEST(FakeClock, AdvanceIsNonDecreasing) {
  constexpr std::uint64_t kD1 = 7;
  constexpr std::uint64_t kD2 = 13;
  FakeClock clock{kT0};

  ExpectStatus(clock.advance(kD1), StatusCode::Ok, FaultCode::NONE);
  const TimestampUs first = clock.now();
  ExpectStatus(clock.advance(kD2), StatusCode::Ok, FaultCode::NONE);
  const TimestampUs second = clock.now();

  EXPECT_GE(first.value, kT0.value);
  EXPECT_GE(second.value, first.value);
  EXPECT_EQ(first.value, kT0.value + kD1);
  EXPECT_EQ(second.value, kT0.value + kD1 + kD2);
}

// TC-SENS-011.3
TEST(FakeClock, AdvanceZeroKeepsEqualRead) {
  FakeClock clock{kT0};
  const TimestampUs before = clock.now();

  ExpectStatus(clock.advance(0), StatusCode::Ok, FaultCode::NONE);

  EXPECT_EQ(clock.now().value, before.value);
}

// TC-SENS-011.4
TEST(FakeClock, SetToCurrentTimeAccepted) {
  FakeClock clock{kT0};

  ExpectStatus(clock.set(kT0), StatusCode::Ok, FaultCode::NONE);

  EXPECT_EQ(clock.now().value, kT0.value);
}

// TC-SENS-011.5
TEST(FakeClock, SetLaterMovesTimeForward) {
  constexpr TimestampUs kT1{kT0.value + 500};
  FakeClock clock{kT0};

  ExpectStatus(clock.set(kT1), StatusCode::Ok, FaultCode::NONE);

  EXPECT_EQ(clock.now().value, kT1.value);
}

// TC-SENS-011.6 (backward-clock injection)
TEST(FakeClock, SetEarlierRejectedTimeUnchanged) {
  constexpr TimestampUs kBackward{kT0.value - 1};
  FakeClock clock{kT0};

  const Status s = clock.set(kBackward);

  ExpectStatus(s, StatusCode::InvalidArgument, FaultCode::TIME_NON_MONOTONIC);
  EXPECT_FALSE(s.is_ok());
  EXPECT_EQ(clock.now().value, kT0.value);
}

// TC-SENS-011.7
TEST(FakeClock, ReadableThroughMonotonicClockInterface) {
  const FakeClock clock{kT0};
  const MonotonicClock& base = clock;

  EXPECT_EQ(base.now().value, kT0.value);
}

// TC-SENS-011.19 (NFR-001 structural evidence)
TEST(FakeClock, MembersAndFaultCodeLookupAreNoexcept) {
  static_assert(noexcept(dfcs::core::to_string(std::declval<FaultCode>())));
  static_assert(noexcept(dfcs::core::fault_code_table()));
  static_assert(noexcept(FakeClock(std::declval<TimestampUs>())));
  static_assert(noexcept(std::declval<const FakeClock&>().now()));
  static_assert(noexcept(std::declval<FakeClock&>().advance(std::declval<std::uint64_t>())));
  static_assert(noexcept(std::declval<FakeClock&>().set(std::declval<TimestampUs>())));
}

// TC-SENS-011.20
TEST(FakeClock, DefaultStartIsZero) {
  const FakeClock clock;
  EXPECT_EQ(clock.now().value, 0U);
}

// TC-SENS-011.21
TEST(FakeClock, ConstructorIsExplicit) {
  static_assert(!std::is_convertible<TimestampUs, FakeClock>::value);
  static_assert(std::is_constructible<FakeClock, TimestampUs>::value);
}

// TC-SENS-009.10
TEST(FakeClock, AdvanceToExactMaximumOk) {
  constexpr std::uint64_t kD = 250;
  FakeClock clock{TimestampUs{kMax - kD}};

  ExpectStatus(clock.advance(kD), StatusCode::Ok, FaultCode::NONE);

  EXPECT_EQ(clock.now().value, kMax);
}

// TC-SENS-009.11 (overflow injection)
TEST(FakeClock, AdvanceOverflowSaturatesAndReportsOutOfRange) {
  constexpr std::uint64_t kD = 250;
  FakeClock clock{TimestampUs{kMax - kD}};

  const Status s = clock.advance(kD + 1);

  ExpectStatus(s, StatusCode::OutOfRange, FaultCode::NONE);
  EXPECT_FALSE(s.is_ok());
  EXPECT_EQ(clock.now().value, kMax);
}

// TC-SENS-009.13 (overflow injection)
TEST(FakeClock, AdvanceOneAtMaximumReportsOutOfRange) {
  FakeClock clock{TimestampUs{kMax}};

  ExpectStatus(clock.advance(1), StatusCode::OutOfRange, FaultCode::NONE);

  EXPECT_EQ(clock.now().value, kMax);
}

// TC-SENS-009.14
TEST(FakeClock, AdvanceZeroAtMaximumOk) {
  FakeClock clock{TimestampUs{kMax}};

  ExpectStatus(clock.advance(0), StatusCode::Ok, FaultCode::NONE);

  EXPECT_EQ(clock.now().value, kMax);
}
