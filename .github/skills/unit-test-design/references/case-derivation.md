# Case derivation rules

## Case quality
- Each case validates one observable behavior through the unit's public interface: return/result values, outputs, public state, and mock calls only where the design specifies them.
- No cases on private members, internal structure or incidental call order.
- Expected results are exact (return code, field value, state). Never "works correctly".
- Deterministic; no real time, I/O or threads (NFR-008). Cases are independent of each other.

## Unit isolation
- Every external dependency (other modules, clock, HAL, drivers, RTOS, OS, filesystem, network, third-party libraries) is replaced by a mock/fake/stub injected via an interface and listed in `Mocks`. Only the unit and the standard library run for real.
- Mock behavior per case is explicit in Preconditions.
- If the design gives no injection seam for a dependency, record a design gap in Notes. Do not write a case that needs the real dependency.

## Required case classes per TC
| Class | Derive from |
|---|---|
| nominal | Main behavior in design section 4 |
| boundary | Limits of ranges, equal/just-past values, saturation, enum edges |
| invalid | Bad arguments, out-of-range enums, malformed input, unsupported commands |
| fault | Injected failures on design-marked safety paths |

## Safety paths
- Inhibit, failsafe, stale sensor, malformed MAVLink: each needs a fault-injection case (`Fault Injection=Y`, Safety Path set).
- Actuator outputs asserted inhibited in BOOT, INITIALIZING, DISARMED and FAULT.
- Unsupported commands: assert actuator state untouched.
- No automatic re-arm after a fault clears.

## TBD handling
- Thresholds, rates, gains, loop timing, battery limits, coverage targets: write `TBD (OD-xxx)` from the design and fill the Open Decisions column. Never invent numbers.
- Use symbolic names or relational checks (`age > stale_limit`) instead of literals.

## Mapping design to rows
1. List every TC in design section 6. Non-unit (pytest, integration, SITL, HIL) become one `deferred` row.
2. Keep design TC IDs and FR/NFR links unchanged. Add a new TC ID only for a design gap: next free in the area, Notes starts with `new`.
3. Split design "Cases" cells (a), (b)... into separate cases; add missing boundary/invalid classes.
4. Reuse existing fakes, fixtures and naming from `tests/unit/*` and prior specs in `docs/test-specs/`.

## Claims
No certification, flight-readiness or safety claims (NFR-017).
