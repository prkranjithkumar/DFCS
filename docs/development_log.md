# DFCS Development Log

## 2026-11-02 - ST-001 host build foundation

- Added root `CMakeLists.txt`, `CMakePresets.json` (`host`, `host-strict`), `cmake/DfcsToolCheck.cmake` and the version manifest `tools/tool-versions.cmake`.
- Added placeholder portable core library `dfcs_core` in `firmware/application` and sample GoogleTest `tests/unit/test_host_build.cpp` (TC-CI-001; FR-070, NFR-010).
- GoogleTest is fetched at commit `52eb8108` (v1.17.0). Pins: CMake 4.4.4, Ninja 1.13.2, GCC 16.2.0 (WinLibs MinGW-w64 UCRT), the versions verified on the dev host. Treat them as proposed.
- Verified locally: `cmake --preset host-strict`, build and `ctest` pass (2/2).
- Open: CI wiring is ST-002; formatter/static analysis are ST-003; Arm toolchain pin is ST-073.
