Module: flight-state and mode manager

- States: BOOT, INITIALIZING, DISARMED, ARMED, FAILSAFE, FAULT. Every transition comes from an explicit table (FR-030, FR-031, SRS section 10); anything not in the table is rejected.
- Reject arming when any pre-arm check fails and report the reason (FR-032, FR-035).
- Reject unsupported or invalid mode commands. Expose only modes that are implemented and tested (FR-033, FR-036).
- Tests: `TC-STATE-NNN`, covering every table row and every failed pre-arm precondition.
