#include "dfcs/status.hpp"

namespace dfcs::core {

namespace {

constexpr const char* kNames[kStatusCodeCount] = {
    "Ok", "InvalidArgument", "OutOfRange", "NotReady", "Timeout",
    "Unsupported", "Rejected", "DeviceError", "Internal",
};

}  // namespace

const char* to_string(StatusCode code) noexcept {
  const auto index = static_cast<std::size_t>(code);
  return index < kStatusCodeCount ? kNames[index] : "UNKNOWN";
}

}  // namespace dfcs::core
