// TC-SENS-011 (ST-005): FaultCode registry values, names, table and to_string.
// Also TC-SENS-010.10: out-of-range FaultCode prints UNKNOWN.
// Verifies: NFR-013

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "dfcs/fault_code.hpp"

namespace {

using dfcs::core::fault_code_table;
using dfcs::core::FaultCode;
using dfcs::core::FaultCodeEntry;
using dfcs::core::kFaultCodeCount;
using dfcs::core::to_string;

using Underlying = std::underlying_type_t<FaultCode>;

struct ExpectedFault {
  FaultCode code;
  std::uint16_t value;
  std::uint8_t category;  // 0 for NONE
  const char* name;
};

// Design 5.3 initial set.
constexpr ExpectedFault kExpected[] = {
    {FaultCode::NONE, 0x0000, 0x00, "NONE"},
    {FaultCode::INIT_STEP_FAILED, 0x0101, 0x01, "INIT_STEP_FAILED"},
    {FaultCode::TIME_NON_MONOTONIC, 0x0201, 0x02, "TIME_NON_MONOTONIC"},
    {FaultCode::SENSOR_STALE, 0x0301, 0x03, "SENSOR_STALE"},
    {FaultCode::SENSOR_OUT_OF_RANGE, 0x0302, 0x03, "SENSOR_OUT_OF_RANGE"},
    {FaultCode::SENSOR_IMPLAUSIBLE, 0x0303, 0x03, "SENSOR_IMPLAUSIBLE"},
    {FaultCode::SENSOR_BUS_ERROR, 0x0304, 0x03, "SENSOR_BUS_ERROR"},
    {FaultCode::ESTIMATOR_INPUT_INVALID, 0x0401, 0x04, "ESTIMATOR_INPUT_INVALID"},
    {FaultCode::CONTROL_INPUT_LOST, 0x0501, 0x05, "CONTROL_INPUT_LOST"},
    {FaultCode::COMM_MALFORMED_MESSAGE, 0x0601, 0x06, "COMM_MALFORMED_MESSAGE"},
    {FaultCode::COMM_UNSUPPORTED_COMMAND, 0x0602, 0x06, "COMM_UNSUPPORTED_COMMAND"},
    {FaultCode::WATCHDOG_TASK_HEALTH, 0x0701, 0x07, "WATCHDOG_TASK_HEALTH"},
    {FaultCode::BATTERY_THRESHOLD, 0x0702, 0x07, "BATTERY_THRESHOLD"},
};
constexpr std::size_t kExpectedCount = sizeof(kExpected) / sizeof(kExpected[0]);

std::uint16_t ValueOf(FaultCode c) { return static_cast<std::uint16_t>(c); }

}  // namespace

// TC-SENS-011.8
TEST(FaultCode, NoneIsZero) {
  EXPECT_EQ(static_cast<Underlying>(FaultCode::NONE), 0U);
}

// TC-SENS-011.9
TEST(FaultCode, NumericValuesMatchRegistry) {
  for (const ExpectedFault& e : kExpected) {
    EXPECT_EQ(ValueOf(e.code), e.value) << e.name;
    if (e.code != FaultCode::NONE) {
      EXPECT_EQ(ValueOf(e.code) >> 8, e.category) << e.name;
    }
  }
}

// TC-SENS-011.10
TEST(FaultCode, NamesMatchRegistry) {
  for (const ExpectedFault& e : kExpected) {
    const char* name = to_string(e.code);
    ASSERT_NE(name, nullptr) << e.name;
    EXPECT_STREQ(name, e.name);
  }
}

// TC-SENS-011.11
TEST(FaultCode, ValuesUnique) {
  const auto table = fault_code_table();
  ASSERT_NE(table.entries, nullptr);
  for (std::size_t i = 0; i < table.size; ++i) {
    for (std::size_t j = i + 1; j < table.size; ++j) {
      EXPECT_NE(ValueOf(table.entries[i].code), ValueOf(table.entries[j].code))
          << i << " vs " << j;
    }
  }
}

// TC-SENS-011.12
TEST(FaultCode, NamesUnique) {
  const auto table = fault_code_table();
  ASSERT_NE(table.entries, nullptr);
  for (std::size_t i = 0; i < table.size; ++i) {
    ASSERT_NE(table.entries[i].name, nullptr) << i;
    for (std::size_t j = i + 1; j < table.size; ++j) {
      ASSERT_NE(table.entries[j].name, nullptr) << j;
      EXPECT_STRNE(table.entries[i].name, table.entries[j].name) << i << " vs " << j;
    }
  }
}

// TC-SENS-011.13
TEST(FaultCode, NamesPrintableAscii) {
  const auto table = fault_code_table();
  ASSERT_NE(table.entries, nullptr);
  for (std::size_t i = 0; i < table.size; ++i) {
    const char* name = table.entries[i].name;
    ASSERT_NE(name, nullptr) << i;
    const std::size_t len = std::strlen(name);
    EXPECT_GE(len, 1U) << i;
    for (std::size_t c = 0; c < len; ++c) {
      EXPECT_GE(static_cast<unsigned char>(name[c]), 0x20U) << name;
      EXPECT_LE(static_cast<unsigned char>(name[c]), 0x7EU) << name;
    }
  }
}

// TC-SENS-011.14 (design gap: digits are not addressed by the naming rule)
TEST(FaultCode, NamesUpperCaseUnderscore) {
  const auto table = fault_code_table();
  ASSERT_NE(table.entries, nullptr);
  for (std::size_t i = 0; i < table.size; ++i) {
    const char* name = table.entries[i].name;
    ASSERT_NE(name, nullptr) << i;
    for (const char* p = name; *p != '\0'; ++p) {
      const bool upper = (*p >= 'A' && *p <= 'Z');
      EXPECT_TRUE(upper || *p == '_') << name;
    }
  }
}

// TC-SENS-011.15
TEST(FaultCode, ToStringRegistryEdges) {
  const char* lowest = to_string(FaultCode::NONE);
  const char* highest = to_string(FaultCode::BATTERY_THRESHOLD);

  ASSERT_NE(lowest, nullptr);
  ASSERT_NE(highest, nullptr);
  EXPECT_STREQ(lowest, "NONE");
  EXPECT_STREQ(highest, "BATTERY_THRESHOLD");
  EXPECT_STRNE(lowest, "UNKNOWN");
  EXPECT_STRNE(highest, "UNKNOWN");
}

// TC-SENS-011.16
TEST(FaultCode, TableSizeEqualsCount) {
  const auto table = fault_code_table();

  EXPECT_EQ(kFaultCodeCount, std::size_t{13});
  EXPECT_EQ(table.size, kFaultCodeCount);
  EXPECT_NE(table.entries, nullptr);
}

// TC-SENS-011.17
TEST(FaultCode, TableContainsEveryEnumeratorOnce) {
  const auto table = fault_code_table();
  ASSERT_NE(table.entries, nullptr);
  ASSERT_EQ(kExpectedCount, std::size_t{13});

  for (const ExpectedFault& e : kExpected) {
    std::size_t found = 0;
    for (std::size_t i = 0; i < table.size; ++i) {
      if (table.entries[i].code == e.code) {
        ++found;
      }
    }
    EXPECT_EQ(found, std::size_t{1}) << e.name;
  }
}

// TC-SENS-011.18
TEST(FaultCode, ToStringMatchesTableEntries) {
  const auto table = fault_code_table();
  ASSERT_NE(table.entries, nullptr);
  for (std::size_t i = 0; i < table.size; ++i) {
    const FaultCodeEntry& entry = table.entries[i];
    const char* text = to_string(entry.code);
    ASSERT_NE(text, nullptr) << i;
    ASSERT_NE(entry.name, nullptr) << i;
    EXPECT_STREQ(text, entry.name) << i;
  }
}

// TC-SENS-010.10 (out-of-range enum injection; 0xFFFF is in the reserved category 0xFF)
TEST(FaultCode, ToStringOutOfRangeIsUnknown) {
  const FaultCode unknown = static_cast<FaultCode>(0xFFFF);

  const auto table = fault_code_table();
  ASSERT_NE(table.entries, nullptr);
  for (std::size_t i = 0; i < table.size; ++i) {
    ASSERT_NE(ValueOf(table.entries[i].code), ValueOf(unknown)) << i;
  }

  const char* text = to_string(unknown);
  ASSERT_NE(text, nullptr);
  EXPECT_STREQ(text, "UNKNOWN");
}
