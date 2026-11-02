Module: Gazebo bridge and simulation adapter

- Implement the same logical interfaces as the hardware drivers (FR-071, FR-072).
- Document message types, units, coordinate frames, timestamps, update rates and failure behavior (SRS 9.4). Frame and unit conversions need tests.
- Lost or late simulator data must surface as stale-sensor faults, never silent defaults.
- Transport and implementation are TBD (OD-011).
- Tests: `TC-SIM-NNN`.
