# DFCS Development Log

## 2026-11-03 - Initial GitHub Actions CI/CD pipeline

- Added `.github/workflows/cicd.yml`: hosted Windows strict host build/tests and reports, explicit blocked HEX packaging, CodeQL C++ and Gitleaks security scans, and an unconditionally disabled AWS deployment placeholder (FR-070, NFR-010, NFR-016, NFR-017).
- Kept existing local tool pins; added hosted Python, checksum-verified WinLibs provisioning, Gitleaks and CodeQL bundle pins to `tools/tool-versions.cmake`. GitHub Actions use immutable commit pins.
- Documented setup, security gates and limitations in `docs/ci-cd.md`. HEX packaging remains blocked by the absent STM32 target and Arm toolchain (ST-073), as agreed. AWS credentials, deployment and firmware flashing are not implemented.
- Verified: actionlint 1.7.12 passes with only the intentional constant-false AWS condition allowed (ShellCheck/Pyflakes unavailable); CMake Tools host build succeeds and all 52 host tests pass; `git diff --check` passes. The hosted `host-strict` workflow, scanner execution and firmware HEX generation are not verified locally. Private-repository CodeQL requires GitHub Code Security entitlement.

## 2026-11-02 - ST-001 host build foundation

- Added root `CMakeLists.txt`, `CMakePresets.json` (`host`, `host-strict`), `cmake/DfcsToolCheck.cmake` and the version manifest `tools/tool-versions.cmake`.
- Added placeholder portable core library `dfcs_core` in `firmware/application` and sample GoogleTest `tests/unit/test_host_build.cpp` (TC-CI-001; FR-070, NFR-010).
- GoogleTest is fetched at commit `52eb8108` (v1.17.0). Pins: CMake 4.4.4, Ninja 1.13.2, GCC 16.2.0 (WinLibs MinGW-w64 UCRT), the versions verified on the dev host. Treat them as proposed.
- Verified locally: `cmake --preset host-strict`, build and `ctest` pass (2/2).
- Open: CI wiring is ST-002; formatter/static analysis are ST-003; Arm toolchain pin is ST-073.

## 2026-11-03 - ST-005 common types and module-interface docs

- Added portable core types under firmware/application: FaultCode (13 codes, table, to_string), StatusCode/Status/Result<T>, TimestampUs/MonotonicClock/elapsed_us. No HAL/FreeRTOS includes, no heap, noexcept (FR-011, NFR-001, NFR-005, NFR-008, NFR-013).
- Acceptance ID for ST-005 is TC-SENS-011 (user decision; TC-SENS-001 stays with SRS 11.3/FR-012). The ST-005 row in docs/DFCS_Stories.md still lists TC-SENS-001 and needs reconciling by the maintainer.
- Unit tests (50 cases, GoogleTest, target dfcs_unit_tests_ST-005) pass on host and host-strict (52/52 with the existing test). Design seams DD-5..DD-8 (kFaultCodeCount, fault_code_table, FakeClock signatures) came from the spec review.
- Added docs/architecture/module-interfaces.md (NFR-009). Open: TC-CI-010 pytest check (Python not installed on dev host), RTM rows in SRS section 17, core_info.cpp comment, OD-005/OD-010/OD-012/OD-014 remain TBD.