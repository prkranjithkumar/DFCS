// FakeClock (ST-005, design 5.2): test-only MonotonicClock implementation; not shipped in the core.
// Verifies: FR-011

#ifndef DFCS_TESTS_UNIT_FAKE_CLOCK_HPP
#define DFCS_TESTS_UNIT_FAKE_CLOCK_HPP

#include <cstdint>
#include <limits>

#include "dfcs/fault_code.hpp"
#include "dfcs/monotonic_clock.hpp"
#include "dfcs/status.hpp"

namespace dfcs::test {

class FakeClock final : public core::MonotonicClock {
 public:
  explicit constexpr FakeClock(core::TimestampUs start = core::TimestampUs{0}) noexcept
      : now_(start) {}

  core::TimestampUs now() const noexcept override { return now_; }

  // Saturates at the maximum and reports OutOfRange when the delta would overflow.
  core::Status advance(std::uint64_t delta_us) noexcept {
    constexpr std::uint64_t kMax = std::numeric_limits<std::uint64_t>::max();
    if (delta_us > kMax - now_.value) {
      now_.value = kMax;
      return core::Status::error(core::StatusCode::OutOfRange);
    }
    now_.value += delta_us;
    return core::Status::ok();
  }

  // Equal time is accepted; an earlier time is rejected and leaves the clock unchanged.
  core::Status set(core::TimestampUs t) noexcept {
    if (t < now_) {
      return core::Status::error(core::StatusCode::InvalidArgument,
                                 core::FaultCode::TIME_NON_MONOTONIC);
    }
    now_ = t;
    return core::Status::ok();
  }

 private:
  core::TimestampUs now_;
};

}  // namespace dfcs::test

#endif  // DFCS_TESTS_UNIT_FAKE_CLOCK_HPP
