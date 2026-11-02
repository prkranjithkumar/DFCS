Module: sensor and actuator drivers

- Implement the same logical interfaces the simulation adapter implements, so portable logic is unchanged between targets (FR-071).
- Timestamp samples from a monotonic source (FR-011). Report bus or device errors as diagnostic faults (FR-013).
- Actuator outputs are inhibited from reset until the safety gate enables them (FR-002).
- Part numbers, pin and peripheral mapping, sample rates and ESC signal parameters are TBD (OD-001, OD-003, OD-005, OD-008); take them from datasheets, never guess.
