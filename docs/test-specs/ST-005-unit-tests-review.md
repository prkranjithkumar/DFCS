# ST-005 Unit Test Spec Review

| Field | Value |
|---|---|
| Story ID | ST-005 |
| Spec | `docs/test-specs/ST-005-unit-tests.csv` |
| Design | `docs/design/ST-005-common-types-and-module-interface-docs.md` |
| Date | 2026-11-03 |
| Verdict | **Changes required** |

POC only; this review makes no certification, flight-readiness or safety claim (NFR-017). Acceptance ID `TC-SENS-011` is a user decision (design 3.3) and is not treated as a defect.

## Summary

| Item | Count |
|---|---|
| Unit cases | 40 (TC-SENS-011: 15, TC-SENS-009: 12, TC-SENS-010: 13), plus 1 deferred row (TC-CI-010) |
| Blockers | 0 |
| Majors | 3 |
| Minors | 7 |

## Check results

| # | Check | Result | Note |
|---|---|---|---|
| 1 | CSV validity | pass | Header equals the template header; 21 columns on every row (manual count; the validator script was not run); Case IDs and Test Names unique; deferred row has empty unit columns. Minor F-6 on `;` inside single-value cells. |
| 2 | Coverage | fail | All design section 7 TCs present (011, 009, 010 unit; CI-010 deferred). FR-011, NFR-013 mapped. Design 7 statement "table covers every enumerator" cannot be verified (F-1). New IDs 009, 010, CI-010 are flagged `new` and are next free (checked against SRS and stories). Minors F-8, F-9, F-10 on link mismatches. |
| 3 | Case quality | fail | Each TC has nominal, boundary and invalid rows. F-2 (unspecified return type makes Expected non-exact), F-3 (redundant case), F-5, F-7. |
| 4 | Safety | pass | Design 5.7 and 7: no actuator path, no inhibit/failsafe/stale-sensor/malformed-input path in ST-005; `Safety Path` is correctly empty. Own failure modes have fault-injection cases (backward time 011.6, 009.4, 009.6, 009.7; out-of-range enums 010.9, 010.10; failed result 010.5; no default-constructed success 010.12). |
| 5 | Behavior and isolation | pass | No external dependency is used by any case (value types, pure function, test-only `FakeClock`); `Mocks` is `none` or lists the stub. No real time, I/O, threads or allocation. Static-assert cases check public type properties only. F-3 notes a misleading mock entry. |
| 6 | TBD handling | pass | Symbolic values throughout; OD-005, OD-012, OD-014 cited where relevant. Numbers used (0x0001, 0x0703, maximum uint8/uint16/uint64, 13 and 9 enumerator counts) trace to design 5.3/5.4 or type limits. F-4 on 0x0703. |
| 7 | Automation readiness | pass | `dfcs_unit_tests` exists in `tests/unit/CMakeLists.txt`; new test files and `fake_clock.hpp` are creatable and must be added to that target. Framework, Target, File and Name set for all 40 unit rows. |

## Findings

| ID | Severity | Case ID | Source | Finding | Required fix |
|---|---|---|---|---|---|
| F-1 | major | TC-SENS-011.9 to .14; TC-SENS-010.8 | Design 7 (TC-SENS-011 (b): "the table covers every enumerator"); CSV Notes of 011.11 | The completeness requirement is not covered by any case. Every case iterates a test-local list copied from design 5.3, so an enumerator added without a table entry (or a table entry for a missing enumerator) is not detected. The spec records this as a design gap but leaves the design requirement unverified. | Raise the gap to Requirement Analysis to add a testable seam (for example a registry count/accessor or sentinel) and then add a case; or have the design explicitly drop or re-scope the "covers every enumerator" statement. Same for `StatusCode` (010.8). |
| F-2 | major | TC-SENS-011.2 to .6; TC-SENS-009.10; TC-SENS-009.11 | Design 5.2 (`FakeClock`: `advance(delta_us)`, `set(t)`) | Expected results ("returns ok", "returns an error (is_ok() is false)") assume a return type that the design does not define for `FakeClock::advance` and `FakeClock::set`. 5.2 says only "returns an error status" for advance overflow and "returns an error" for set. The Expected value is therefore not exact or observable as written. | Obtain a signature decision for `advance` and `set` (return type, and StatusCode/FaultCode of the errors) from Requirement Analysis, then restate Expected exactly. Correct the Notes of 009.11, which claim the return type is unspecified although 5.2 states a status is returned. |
| F-3 | major | TC-SENS-009.8 | Design 5.2 (`elapsed_us` takes `TimestampUs`, not a clock); case rules (one behavior per case) | The scripted-clock stub only supplies two timestamps; the behavior under test is `elapsed_us(later < earlier)`, identical to 009.6. The listed mock `MonotonicClock(stub)` is not consumed by any unit under test, and the Notes claim a missing injection seam that does not exist for this behavior. | Remove the case, or redefine it against a real unit that consumes a clock (none exists in ST-005) and move it to the owning story (ST-021). |
| F-4 | minor | TC-SENS-010.10 | Design 5.3 (append-only; owner stories add codes) | The value 0x0703 is asserted as "unknown", but 5.3 allows owner stories to append codes in category 0x07. A legitimate append would fail this test. | Express the out-of-range value relationally (a value not present in the registry list) or use a value outside all categories derived from 5.3. |
| F-5 | minor | TC-SENS-010.6 | Case quality | Type `boundary` is a mislabel: `value_or` on a failed result is a nominal failure path, not a boundary. | Relabel as `invalid` or `nominal`, or add a real boundary. |
| F-6 | minor | TC-SENS-009.8, 009.12, 010.3, 010.5 | CSV format (`;` is the multi-value separator) | `;` is used as an argument separator inside Stimulus/Preconditions text, for example `Status::error(DeviceError; SENSOR_BUS_ERROR)`. This is ambiguous with the multi-value separator. | Use `,` inside quoted text. |
| F-7 | minor | TC-SENS-011.9 | Design 5.3 table | Expected says "equals its table value" without listing the values; the case is not self-contained. | Reference the design table by row or list the value/name pairs. |
| F-8 | minor | TC-SENS-010.4 | Design 6/7 (NFR-009), SRS NFR-009 (review) | NFR-009 (documented interfaces and ownership) is cited on a value-holding case that does not verify documentation. The only meaningful verification is deferred TC-CI-010 plus review. | Keep the link only if the design requires it; otherwise note that unit-level evidence for NFR-009 is by-copy only. |
| F-9 | minor | TC-SENS-011.7, TC-SENS-009.8 | Design 7 and 8 (FR/NFR verified) | FR-071 is cited but is not in the design 7 FR/NFR column or the RTM rows for these TCs (it is listed only as related, 3.2). | Remove FR-071 or have the design add it to section 7/8. |
| F-10 | minor | TC-SENS-010.13 | Design 6 (NFR-001 verified by TC-SENS-011, TC-SENS-009) | NFR-001 is cited on a TC-SENS-010 case; the design lists TC-SENS-011 and TC-SENS-009 for NFR-001, and TC-SENS-011 has no NFR-001 case. | Align design 6 and the CSV on which TCs verify NFR-001. |

## Coverage matrix

| FR/NFR | TC IDs | Case IDs |
|---|---|---|
| FR-011 | TC-SENS-011, TC-SENS-009 | 011.1 to 011.7; 009.1 to 009.12 |
| NFR-013 | TC-SENS-011, TC-SENS-010 | 011.8 to 011.15; 010.1 to 010.13 |
| NFR-009 | TC-CI-010 (deferred), TC-SENS-010 | deferred row; 010.4 (see F-8) |
| FR-071 (related) | TC-SENS-011, TC-SENS-009 | 011.7; 009.8 (see F-9) |
| NFR-001 (related) | TC-SENS-009, TC-SENS-010 | 009.12; 010.13 (see F-10) |
| NFR-005 (related) | TC-SENS-010 | 010.11 (by construction only) |
| NFR-008 (related) | TC-SENS-010 | 010.11 (include ban enforced by TC-CI-004, ST-004) |
