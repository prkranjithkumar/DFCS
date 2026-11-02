cmake_minimum_required(VERSION 3.25)

project(DFCS VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(tools/tool-versions.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(DfcsToolCheck)

option(DFCS_BUILD_TESTS "Build host unit tests" ON)

# Portable core only; firmware/hal, drivers and rtos are not part of the host build (NFR-008).
add_subdirectory(firmware/application)

if(DFCS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/unit)
endif()
