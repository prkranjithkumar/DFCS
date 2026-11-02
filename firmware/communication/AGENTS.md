Module: MAVLink 2 telemetry and command handling

- Parse only supported messages. Validate length, source and command parameters (FR-051). Use bounded buffers; no unbounded input handling.
- Reject unsupported commands without touching actuator state (FR-054).
- Parsing must not block time-critical control (FR-055).
- The supported message subset is TBD (OD-010). Do not treat QGroundControl as compatible until an interoperability test passes (SRS 9.3).
- Evaluate message signing or an authenticated command channel before any radio deployment (FR-056).
- Tests: `TC-COMM-NNN`, with malformed, oversized and unsupported-message negative tests.
