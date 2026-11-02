Module: safety manager (arming checks, output inhibition, failsafe, watchdog supervision)

- Owns watchdog policy and the failsafe response; `rtos/` only wires task-health reporting (FR-064). Reach the hardware motor-disable path and watchdog through interfaces (FR-060, OD-013).
- The output gate must inhibit actuators independently of the control calculation (SRS 4.3, FR-046).
- Unknown, invalid or uninitialized state resolves to inhibited outputs.
- Each fault class has one deterministic, documented response (FR-066). The failsafe actuator response is TBD pending safety analysis; do not assume a motor stop is safe.
- Leaving FAILSAFE or FAULT needs explicit, validated recovery conditions. Never auto re-arm (FR-067).
- Tests: `TC-SAFE-NNN`, with fault injection for every inhibit and failsafe path.
