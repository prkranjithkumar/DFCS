#pragma once

// Portable core: must not include STM32 HAL or FreeRTOS headers (NFR-008).

#include <cstdint>

#include "dfcs/fault_code.hpp"
#include "dfcs/status.hpp"

namespace dfcs::core {

// Microseconds since an arbitrary origin; resolution TBD (OD-005).
struct TimestampUs {
  std::uint64_t value;
};

constexpr bool operator==(TimestampUs a, TimestampUs b) noexcept { return a.value == b.value; }
constexpr bool operator!=(TimestampUs a, TimestampUs b) noexcept { return a.value != b.value; }
constexpr bool operator<(TimestampUs a, TimestampUs b) noexcept { return a.value < b.value; }
constexpr bool operator<=(TimestampUs a, TimestampUs b) noexcept { return a.value <= b.value; }
constexpr bool operator>(TimestampUs a, TimestampUs b) noexcept { return a.value > b.value; }
constexpr bool operator>=(TimestampUs a, TimestampUs b) noexcept { return a.value >= b.value; }

// Implementations: non-decreasing, never wall-clock, non-blocking, bounded, no allocation or I/O.
class MonotonicClock {
 public:
  virtual TimestampUs now() const noexcept = 0;

 protected:
  ~MonotonicClock() = default;
};

// A backward pair returns TIME_NON_MONOTONIC, never zero or a wrapped value.
constexpr Result<std::uint64_t> elapsed_us(TimestampUs later, TimestampUs earlier) noexcept {
  if (later < earlier) {
    return Result<std::uint64_t>::failure(
        Status::error(StatusCode::InvalidArgument, FaultCode::TIME_NON_MONOTONIC));
  }
  return Result<std::uint64_t>::success(later.value - earlier.value);
}

}  // namespace dfcs::core
