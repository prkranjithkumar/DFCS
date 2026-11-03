// TC-SENS-010 (ST-005): Status, Result<T> and StatusCode behavior; value-type static checks.
// Verifies: NFR-013, NFR-001 (structural), NFR-005, NFR-008 (by-construction evidence)

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>
#include <utility>

#include "dfcs/fault_code.hpp"
#include "dfcs/monotonic_clock.hpp"
#include "dfcs/status.hpp"

namespace {

using dfcs::core::FaultCode;
using dfcs::core::kStatusCodeCount;
using dfcs::core::Result;
using dfcs::core::Status;
using dfcs::core::StatusCode;
using dfcs::core::TimestampUs;
using dfcs::core::to_string;

using U64Result = Result<std::uint64_t>;

StatusCode StatusCodeFromIndex(std::size_t i) { return static_cast<StatusCode>(i); }

}  // namespace

// TC-SENS-010.1
TEST(Status, OkFactory) {
  const Status s = Status::ok();

  EXPECT_EQ(s.code, StatusCode::Ok);
  EXPECT_EQ(s.fault, FaultCode::NONE);
  EXPECT_TRUE(s.is_ok());
}

// TC-SENS-010.2
TEST(Status, ErrorDefaultsFaultToNone) {
  const Status s = Status::error(StatusCode::InvalidArgument);

  EXPECT_EQ(s.code, StatusCode::InvalidArgument);
  EXPECT_EQ(s.fault, FaultCode::NONE);
  EXPECT_FALSE(s.is_ok());
}

// TC-SENS-010.3
TEST(Status, ErrorKeepsCodeAndFault) {
  const Status s = Status::error(StatusCode::DeviceError, FaultCode::SENSOR_BUS_ERROR);

  EXPECT_EQ(s.code, StatusCode::DeviceError);
  EXPECT_EQ(s.fault, FaultCode::SENSOR_BUS_ERROR);
  EXPECT_FALSE(s.is_ok());
}

// TC-SENS-010.4
TEST(Result, SuccessHoldsValue) {
  constexpr std::uint64_t kValue = 4242;
  constexpr std::uint64_t kFallback = 7;

  const U64Result r = U64Result::success(kValue);

  EXPECT_TRUE(r.is_ok());
  EXPECT_TRUE(r.status().is_ok());
  EXPECT_EQ(r.status().fault, FaultCode::NONE);
  EXPECT_EQ(r.value(), kValue);
  EXPECT_EQ(r.value_or(kFallback), kValue);
}

// TC-SENS-010.5 (failed-result injection)
TEST(Result, FailureCarriesStatus) {
  const Status s = Status::error(StatusCode::DeviceError, FaultCode::SENSOR_BUS_ERROR);

  const U64Result r = U64Result::failure(s);

  EXPECT_FALSE(r.is_ok());
  EXPECT_EQ(r.status().code, StatusCode::DeviceError);
  EXPECT_EQ(r.status().fault, FaultCode::SENSOR_BUS_ERROR);
}

// TC-SENS-010.6
TEST(Result, ValueOrReturnsFallbackOnFailure) {
  constexpr std::uint64_t kFallback = 99;

  const U64Result r = U64Result::failure(Status::error(StatusCode::Timeout));

  EXPECT_EQ(r.value_or(kFallback), kFallback);
}

// TC-SENS-010.7 (the FaultCode of the coerced status is unspecified by the design; not asserted)
TEST(Result, FailureWithOkCoercedToInternal) {
  const U64Result r = U64Result::failure(Status::ok());

  EXPECT_FALSE(r.is_ok());
  EXPECT_EQ(r.status().code, StatusCode::Internal);
}

// TC-SENS-010.8
TEST(StatusCode, ToStringPrintableAndUnique) {
  for (std::size_t i = 0; i < kStatusCodeCount; ++i) {
    const char* name = to_string(StatusCodeFromIndex(i));
    ASSERT_NE(name, nullptr) << "index " << i;
    const std::size_t len = std::strlen(name);
    EXPECT_GE(len, 1U) << "index " << i;
    for (std::size_t c = 0; c < len; ++c) {
      EXPECT_GE(static_cast<unsigned char>(name[c]), 0x20U) << "index " << i;
      EXPECT_LE(static_cast<unsigned char>(name[c]), 0x7EU) << "index " << i;
    }
    EXPECT_STRNE(name, "UNKNOWN") << "index " << i;

    for (std::size_t j = i + 1; j < kStatusCodeCount; ++j) {
      EXPECT_STRNE(name, to_string(StatusCodeFromIndex(j))) << i << " vs " << j;
    }
  }
}

// TC-SENS-010.9 (out-of-range enum injection)
TEST(StatusCode, ToStringOutOfRangeIsUnknown) {
  const char* at_count = to_string(StatusCodeFromIndex(kStatusCodeCount));
  const char* at_max = to_string(static_cast<StatusCode>(std::numeric_limits<std::uint8_t>::max()));

  ASSERT_NE(at_count, nullptr);
  ASSERT_NE(at_max, nullptr);
  EXPECT_STREQ(at_count, "UNKNOWN");
  EXPECT_STREQ(at_max, "UNKNOWN");
}

// TC-SENS-010.11
TEST(Types, ValueTypesAreTriviallyCopyable) {
  static_assert(std::is_trivially_copyable<Status>::value);
  static_assert(std::is_trivially_copyable<U64Result>::value);
  static_assert(std::is_trivially_copyable<TimestampUs>::value);
  static_assert(std::is_trivially_copyable<FaultCode>::value);
  static_assert(std::is_trivially_copyable<StatusCode>::value);
}

// TC-SENS-010.12
TEST(Types, StatusAndResultNotDefaultConstructible) {
  static_assert(!std::is_default_constructible<Status>::value);
  static_assert(!std::is_default_constructible<U64Result>::value);
}

// TC-SENS-010.13 (to_string(FaultCode) noexcept is covered by TC-SENS-011.19)
TEST(Types, AccessorsAreNoexcept) {
  static_assert(noexcept(Status::ok()));
  static_assert(noexcept(Status::error(std::declval<StatusCode>())));
  static_assert(noexcept(std::declval<const Status&>().is_ok()));
  static_assert(noexcept(U64Result::success(std::declval<std::uint64_t>())));
  static_assert(noexcept(U64Result::failure(std::declval<Status>())));
  static_assert(noexcept(std::declval<const U64Result&>().status()));
  static_assert(noexcept(std::declval<const U64Result&>().value_or(std::declval<std::uint64_t>())));
  static_assert(noexcept(std::declval<const U64Result&>().value()));
  static_assert(noexcept(to_string(std::declval<StatusCode>())));
}

// TC-SENS-010.14
TEST(StatusCode, CountIsOnePastInternal) {
  using Underlying = std::underlying_type_t<StatusCode>;

  EXPECT_EQ(kStatusCodeCount, std::size_t{9});
  EXPECT_EQ(kStatusCodeCount, static_cast<std::size_t>(static_cast<Underlying>(StatusCode::Internal)) + 1U);
}
