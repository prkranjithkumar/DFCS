---
name: unit-test-design
description: "Produce the unit test specification CSV for a DFCS story. Use when given a Story ID (e.g. ST-010) whose design exists in docs/design, to write docs/test-specs/<STORY-ID>-unit-tests.csv: test cases per design section 6 TC, mocks, fault-injection and safety-path cases, FR/NFR traceability. Spec only; no test code or implementation."
argument-hint: "Story ID, e.g. ST-010"
---

# Unit Test Design (DFCS)

Input: Story ID. Reads the design from Requirement Analysis; outputs one unit test spec as CSV. Spec only: no test code, no implementation.

## When to Use
- A design exists at `docs/design/<STORY-ID>-*.md` and the story needs a unit test spec before review and automation.
- Re-running to extend an existing `docs/test-specs/<STORY-ID>-unit-tests.csv` (keep existing Case IDs).

## Sources (priority order)
1. `docs/design/<STORY-ID>-*.md` (sections 2 requirements, 4 design, 6 test design, 7 RTM, 8 open decisions). If missing, stop and tell the user to run Requirement Analysis first.
2. `docs/DFCS_Stories.md` (acceptance, test IDs)
3. `docs/Drone_Flight_Control_System_SRS.md` (FR/NFR, 11 verification, 17 RTM)
4. Root `AGENTS.md`, `AGENTS.md` of touched modules, `tests/AGENTS.md`
5. Existing `docs/test-specs/*`, `tests/unit/*` (reuse fakes, naming)

## Rules
- Write only `docs/test-specs/<STORY-ID>-unit-tests.csv`. Never edit design, SRS, stories, code or tests.
- Cover every design section 6 TC at unit level (GoogleTest host C++, Unity embedded C). Non-unit TCs get one `deferred` row, no cases.
- Keep design TC IDs and FR/NFR links; new IDs only for design gaps, next free in the area, flagged `new`.
- Every external dependency is mocked via an injected interface and listed in `Mocks`; no real time, I/O or threads.
- Never invent thresholds, rates, gains, timing or coverage targets: `TBD (OD-xxx)` from the design.
- Ask the user only if the design is missing or contradicts the SRS/story; never resolve silently.
- No certification, flight-readiness or safety claims (NFR-017).

Details: [case derivation rules](./references/case-derivation.md).

## Procedure
1. Read the design, story, cited SRS parts, AGENTS.md files, existing specs and tests.
2. Map each design TC to unit-level cases (or a deferred row). Follow [case derivation rules](./references/case-derivation.md).
3. Draft the CSV from [the template](./assets/unit-tests.template.csv) following [CSV format](./references/csv-format.md) (schema: [unit-test-spec.schema.json](./schemas/unit-test-spec.schema.json)).
4. Validate: `powershell -NoProfile -File .github/skills/unit-test-design/scripts/Test-UnitTestSpec.ps1 -Path docs/test-specs/<STORY-ID>-unit-tests.csv` (auto-finds the design; override with `-Design`). Fix all errors and re-run until exit code 0.
5. Walk the [completion checklist](./references/checklist.md) for the manual items.

## Final response
- CSV file path
- Checklist results (pass/fail per item)
- 2-3 lines: design gaps or TBDs needing decisions

Next step: Unit Test Review.
