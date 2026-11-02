Project title: Drone Flight Control System (DFCS)

Project description:
- Proof-of-concept Quad-X flight-control firmware for STM32H743, developed simulation-first.
- Source of truth: `docs/Drone_Flight_Control_System_SRS.md`. Cite FR/NFR/OD IDs; do not restate it here.
- Not certified, production-ready, or inherently safe, and never authorization to fly. Do not claim otherwise (NFR-017).

Folder Structure:
- `docs/` holds the SRS. `README.md` is empty. No source code exists yet.
- Module folders follow SRS Appendix A. Each has its own `AGENTS.md` with module-only rules, and the closest file to the edited code applies:
  - `firmware/`: `application`, `control`, `estimation`, `safety`, `communication`, `drivers`, `hal`, `rtos`.
  - `simulation/`: `gazebo`, `bridge`, `scenarios`.
  - `tests/`: one `AGENTS.md`; subfolders are created with the first test.
- `firmware/navigation/` is intentionally absent: autonomous missions are out of scope (SRS 2.3).
- Keep only global rules in this file.

Tech stack (SRS baseline, proposed; versions not yet pinned):
- C / C++17, FreeRTOS, STM32CubeH7 HAL/LL, CMake + Ninja, GNU Arm Embedded Toolchain.
- MAVLink 2, Gazebo Harmonic, GitHub Actions.
- Architecture rules:
  - Control, estimation, state machine, mixer and safety logic must not include STM32 HAL or FreeRTOS headers (NFR-008). Hardware and simulation adapters implement the same interfaces.
  - No dynamic allocation after init and no blocking I/O in time-critical paths (NFR-001, NFR-005).
  - Actuator outputs stay inhibited in BOOT, INITIALIZING, DISARMED and FAULT. No automatic re-arm. Unsupported commands are rejected without touching actuator state.
  - Do not invent gains, loop rates, mixer coefficients, pin maps, battery thresholds or timing/coverage targets. They are open decisions (OD-xxx); mark them TBD.
  - Do not advertise flight modes that are not implemented and tested.

Testing framework along with examples:
- GoogleTest (host C++), Unity (embedded C), pytest (automation, SITL).
- Test IDs: `TC-<AREA>-NNN` with areas BOOT, SAFE, SENS, EST, STATE, CTRL, MIX, COMM, SIM, CI. Example: `TC-STATE-001` rejects arming when a pre-arm check fails.
- Each test cites the FR/NFR it verifies; update the RTM (SRS section 17) with it.
- Safety paths (inhibit, failsafe, stale sensor, malformed MAVLink) require fault-injection tests.
- Host build and test: `cmake --preset host`, `cmake --build --preset host`, `ctest --preset host` (CI uses `host-strict`). Pinned tool versions: `tools/tool-versions.cmake`.

Linting and formatting:
- Cppcheck and Clang-Tidy are the proposed static analysis; critical findings block CI.
- Formatter, coding standard (MISRA is guidance only) and config files are not yet chosen. Do not add them silently; propose first.
- Pin tool versions in a manifest; never let them float in CI (SRS section 5).

Simulation standards:
- Portable core must build for the desktop host and run against Gazebo through the same interfaces as hardware drivers.
- The Gazebo bridge documents message types, units, coordinate frames, timestamps, update rates and failure behavior. Frame and unit conversions need tests.
- Scenarios record their configuration and seed so runs are repeatable.

External library references:
- Use official STM32, FreeRTOS, MAVLink, Gazebo and toolchain documentation.
- MISRA, IEC 61508, ISO 26262 and OWASP are engineering references only; make no compliance claims.

Handling sensitive data:
- No credentials, private keys or secrets in source control.
- Validate all externally supplied message fields; fail safely on malformed input (NFR-016).
- Firmware flashing and flight authorization are never automated in CI.
- Bench work only with propulsion disabled, propellers removed and power isolated.
