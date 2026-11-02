# Compares the tools in use against tools/tool-versions.cmake (NFR-010).

option(DFCS_STRICT_TOOL_VERSIONS "Fail configure when a tool differs from its pinned version" OFF)

function(dfcs_check_pin tool actual pinned)
  if(actual STREQUAL pinned)
    message(STATUS "DFCS tool ${tool}: ${actual} (matches pin)")
    return()
  endif()
  set(msg "DFCS tool ${tool}: found '${actual}', pinned '${pinned}'")
  if(DFCS_STRICT_TOOL_VERSIONS)
    message(FATAL_ERROR "${msg}")
  else()
    message(WARNING "${msg}")
  endif()
endfunction()

dfcs_check_pin("cmake" "${CMAKE_VERSION}" "${DFCS_PIN_CMAKE}")

if(CMAKE_GENERATOR MATCHES "Ninja")
  execute_process(
    COMMAND "${CMAKE_MAKE_PROGRAM}" --version
    OUTPUT_VARIABLE dfcs_ninja_version
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE dfcs_ninja_rc)
  if(NOT dfcs_ninja_rc EQUAL 0)
    set(dfcs_ninja_version "unknown")
  endif()
  dfcs_check_pin("ninja" "${dfcs_ninja_version}" "${DFCS_PIN_NINJA}")
else()
  message(WARNING "DFCS expects the Ninja generator (SRS section 5); using '${CMAKE_GENERATOR}'")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  dfcs_check_pin("g++" "${CMAKE_CXX_COMPILER_VERSION}" "${DFCS_PIN_GCC}")
else()
  message(STATUS "DFCS host compiler ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}: no pin defined")
endif()
