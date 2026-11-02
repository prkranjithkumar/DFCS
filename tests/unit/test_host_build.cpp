// TC-CI-001 (ST-001): sample host test proving the portable core builds and runs on the desktop.
// Verifies: FR-070 (host-buildable core), NFR-010 (reproducible build and test steps).

#include <gtest/gtest.h>

#include <cstring>

#include "dfcs/core_info.hpp"

TEST(HostBuild, CoreLibraryLinksAndRuns) {
  EXPECT_STREQ(dfcs::core::project_name(), "DFCS");
}

TEST(HostBuild, ProjectNameIsNotEmpty) {
  EXPECT_GT(std::strlen(dfcs::core::project_name()), 0U);
}
