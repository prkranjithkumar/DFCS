include(FetchContent)

# Pinned by commit hash (tools/tool-versions.cmake) so CI cannot float to a newer release.
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG ${DFCS_PIN_GOOGLETEST_COMMIT})

# Use the dynamic MSVC runtime so gtest links with the project on Windows hosts.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(dfcs_unit_tests test_host_build.cpp)
target_link_libraries(dfcs_unit_tests PRIVATE dfcs::core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(dfcs_unit_tests)
