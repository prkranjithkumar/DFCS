#pragma once

// Portable core: must not include STM32 HAL or FreeRTOS headers (NFR-008).

#include <cstddef>
#include <cstdint>

#include "dfcs/fault_code.hpp"

namespace dfcs::core {

// Contiguous from 0; Internal stays last. Append before raising kStatusCodeCount.
enum class StatusCode : std::uint8_t {
  Ok,
  InvalidArgument,
  OutOfRange,
  NotReady,
  Timeout,
  Unsupported,
  Rejected,
  DeviceError,
  Internal,
};

inline constexpr std::size_t kStatusCodeCount = 9;

// Never null; a value at or above kStatusCodeCount returns "UNKNOWN".
const char* to_string(StatusCode code) noexcept;

// No default constructor: an uninitialized outcome cannot read as success.
struct [[nodiscard]] Status {
  StatusCode code;
  FaultCode fault;

  static constexpr Status ok() noexcept { return Status{StatusCode::Ok, FaultCode::NONE}; }

  static constexpr Status error(StatusCode code, FaultCode fault = FaultCode::NONE) noexcept {
    return Status{code, fault};
  }

  constexpr bool is_ok() const noexcept { return code == StatusCode::Ok; }

 private:
  constexpr Status(StatusCode c, FaultCode f) noexcept : code(c), fault(f) {}
};

// T must be trivially copyable and default-constructible.
template <class T>
class [[nodiscard]] Result {
 public:
  static constexpr Result success(T value) noexcept { return Result(Status::ok(), value); }

  // A failure carrying an Ok status is coerced to Internal.
  static constexpr Result failure(Status status) noexcept {
    return Result(status.is_ok() ? Status::error(StatusCode::Internal) : status, T{});
  }

  constexpr bool is_ok() const noexcept { return status_.is_ok(); }
  constexpr Status status() const noexcept { return status_; }
  constexpr T value_or(T fallback) const noexcept { return is_ok() ? value_ : fallback; }

  // Precondition: is_ok().
  constexpr const T& value() const noexcept { return value_; }

 private:
  constexpr Result(Status status, T value) noexcept : status_(status), value_(value) {}

  Status status_;
  T value_;
};

}  // namespace dfcs::core
