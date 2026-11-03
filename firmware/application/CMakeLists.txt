# Portable core library. core_info is a placeholder so the host build has something to link and test.
add_library(dfcs_core STATIC
  src/core_info.cpp
  src/fault_code.cpp
  src/status.cpp)
add_library(dfcs::core ALIAS dfcs_core)

target_include_directories(dfcs_core PUBLIC include)
target_compile_features(dfcs_core PUBLIC cxx_std_17)

# Warnings only; -Werror and the coding standard are undecided (ST-003.a).
target_compile_options(dfcs_core PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>)
