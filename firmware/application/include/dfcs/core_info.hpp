#pragma once

// Portable core: must not include STM32 HAL or FreeRTOS headers (NFR-008).

namespace dfcs::core {

// Project name; placeholder until real core types land (ST-005).
const char* project_name() noexcept;

}  // namespace dfcs::core
