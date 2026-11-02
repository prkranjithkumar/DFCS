Module: attitude control (PID) and Quad-X mixer

- Consume only validated state estimates and valid command inputs (FR-041). Invalid input or an active inhibit yields inhibited or defined-safe outputs (FR-046).
- Apply configured output limits and anti-windup (FR-042). Bound mixer output to configured actuator limits (FR-044).
- Gains, limits, motor order, rotation direction and mixer signs live in versioned configuration, never hard-coded (FR-045, FR-047). Values are TBD (OD-007, OD-009); do not invent them.
- Tests: `TC-CTRL-NNN` and `TC-MIX-NNN`, covering saturation, anti-windup, sign and ordering, and output bounds.
