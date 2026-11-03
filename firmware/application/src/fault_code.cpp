#include "dfcs/fault_code.hpp"

namespace dfcs::core {

namespace {

constexpr FaultCodeEntry kTable[kFaultCodeCount] = {
    {FaultCode::NONE, "NONE"},
    {FaultCode::INIT_STEP_FAILED, "INIT_STEP_FAILED"},
    {FaultCode::TIME_NON_MONOTONIC, "TIME_NON_MONOTONIC"},
    {FaultCode::SENSOR_STALE, "SENSOR_STALE"},
    {FaultCode::SENSOR_OUT_OF_RANGE, "SENSOR_OUT_OF_RANGE"},
    {FaultCode::SENSOR_IMPLAUSIBLE, "SENSOR_IMPLAUSIBLE"},
    {FaultCode::SENSOR_BUS_ERROR, "SENSOR_BUS_ERROR"},
    {FaultCode::ESTIMATOR_INPUT_INVALID, "ESTIMATOR_INPUT_INVALID"},
    {FaultCode::CONTROL_INPUT_LOST, "CONTROL_INPUT_LOST"},
    {FaultCode::COMM_MALFORMED_MESSAGE, "COMM_MALFORMED_MESSAGE"},
    {FaultCode::COMM_UNSUPPORTED_COMMAND, "COMM_UNSUPPORTED_COMMAND"},
    {FaultCode::WATCHDOG_TASK_HEALTH, "WATCHDOG_TASK_HEALTH"},
    {FaultCode::BATTERY_THRESHOLD, "BATTERY_THRESHOLD"},
};

}  // namespace

FaultCodeTable fault_code_table() noexcept {
  return FaultCodeTable{kTable, kFaultCodeCount};
}

const char* to_string(FaultCode code) noexcept {
  for (const FaultCodeEntry& entry : kTable) {
    if (entry.code == code) {
      return entry.name;
    }
  }
  return "UNKNOWN";
}

}  // namespace dfcs::core
