#pragma once

// Portable core: must not include STM32 HAL or FreeRTOS headers (NFR-008).

#include <cstddef>
#include <cstdint>

namespace dfcs::core {

// High byte is the category, low byte the item. Append-only; category 0xFF is reserved and never allocated.
// Initial set is proposed by ST-005 and may be refined by owner stories. FaultCode, kFaultCodeCount and
// the table in fault_code.cpp change together.
enum class FaultCode : std::uint16_t {
  NONE = 0x0000,
  INIT_STEP_FAILED = 0x0101,
  TIME_NON_MONOTONIC = 0x0201,
  SENSOR_STALE = 0x0301,
  SENSOR_OUT_OF_RANGE = 0x0302,
  SENSOR_IMPLAUSIBLE = 0x0303,
  SENSOR_BUS_ERROR = 0x0304,
  ESTIMATOR_INPUT_INVALID = 0x0401,
  CONTROL_INPUT_LOST = 0x0501,
  COMM_MALFORMED_MESSAGE = 0x0601,
  COMM_UNSUPPORTED_COMMAND = 0x0602,
  WATCHDOG_TASK_HEALTH = 0x0701,
  BATTERY_THRESHOLD = 0x0702,  // threshold value TBD (OD-014)
};

inline constexpr std::size_t kFaultCodeCount = 13;

struct FaultCodeEntry {
  FaultCode code;
  const char* name;
};

struct FaultCodeTable {
  const FaultCodeEntry* entries;
  std::size_t size;
};

// Read-only table in static storage; the one `to_string` searches.
FaultCodeTable fault_code_table() noexcept;

// Never null; an unregistered value returns "UNKNOWN".
const char* to_string(FaultCode code) noexcept;

}  // namespace dfcs::core
