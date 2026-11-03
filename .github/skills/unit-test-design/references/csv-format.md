# CSV format

Authoritative machine-readable contract: [unit-test-spec.schema.json](../schemas/unit-test-spec.schema.json). Starting point: [unit-tests.template.csv](../assets/unit-tests.template.csv).

## File
- Path: `docs/test-specs/<STORY-ID>-unit-tests.csv`. UTF-8 without BOM, comma-separated, one header row, one row per case.
- RFC 4180: quote fields containing comma, quote or newline; double inner quotes. No blank rows, no comments, no newlines inside cells.
- Multi-value cells use `;` (e.g. `FR-011;NFR-013`).

## Columns (in order)
`Story ID,TC ID,Case ID,Level,Title,Requirements,Design Section,Framework,Test Target,Test File,Test Name,Preconditions,Mocks,Stimulus,Expected,Type,Fault Injection,Safety Path,Open Decisions,Status,Notes`

| Column | Rule |
|---|---|
| Story ID | `ST-NNN` |
| TC ID | `TC-<AREA>-NNN`; AREA in BOOT, SAFE, SENS, EST, STATE, CTRL, MIX, COMM, SIM, CI. Design IDs unchanged |
| Case ID | `<TC-ID>.<n>`, e.g. `TC-SENS-009.3`. Empty for deferred rows |
| Level | `unit` or `deferred` (pytest, integration, SITL, HIL; reason in Notes) |
| Title | Short, one behavior |
| Requirements | `;`-separated FR/NFR IDs from the design |
| Design Section | Design doc section the case derives from, e.g. `4.2` |
| Framework | `GoogleTest` (host C++), `Unity` (embedded C), `pytest` |
| Test Target | CMake target |
| Test File | `tests/unit/<file>` |
| Test Name | `Suite.Case` proposal, unique across the file |
| Preconditions | Initial state and explicit mock behavior (returned values, injected faults). Empty for deferred |
| Mocks | `;`-separated `Interface(mock\|fake\|stub)`; `none` if the unit has no dependencies |
| Stimulus | Single call/event applied to the unit. Empty for deferred |
| Expected | Exact observable result (return code, field value, state, mock calls the design specifies). Empty for deferred |
| Type | `nominal`, `boundary`, `invalid`, `fault` |
| Fault Injection | `Y` or `N` |
| Safety Path | `inhibit`, `failsafe`, `stale-sensor`, `malformed-input`, or empty |
| Open Decisions | `TBD (OD-xxx)` refs, `;`-separated, or empty |
| Status | `Planned` |
| Notes | Fakes/fixtures, `new` flag for new IDs, design gaps |

## Deferred rows
One row per non-unit TC from design section 6. `Level=deferred`; Case ID, Preconditions, Stimulus, Expected empty; reason in Notes; no cases.

## Cross rules
- Case ID begins with its TC ID.
- Type `fault` or any Safety Path implies `Fault Injection=Y`.
- Each TC with unit rows has `nominal`, `boundary` and `invalid` cases.
- A TC ID absent from design section 6 is a new ID: next free in its area, Notes contains `new`.
