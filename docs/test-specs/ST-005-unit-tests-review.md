# ST-005 Unit Test Spec Review

## Review round 2

| Field | Value |
|---|---|
| Story ID | ST-005 |
| Spec | `docs/test-specs/ST-005-unit-tests.csv` |
| Design | `docs/design/ST-005-common-types-and-module-interface-docs.md` (updated, section 9 DD-5 to DD-8) |
| Previous report | Round 1: 0 blockers, 3 majors (F-1 to F-3), 7 minors (F-4 to F-10), verdict `Changes required` |
| Date | 2026-11-03 |
| Verdict | **Approved** |

POC only; this review makes no certification, flight-readiness or safety claim (NFR-017). Acceptance ID `TC-SENS-011` is a user decision (design 3.3) and is not treated as a defect.

## Summary

| Item | Count |
|---|---|
| Unit cases | 50 (TC-SENS-011: 21, TC-SENS-009: 15, TC-SENS-010: 14), plus 1 deferred row (TC-CI-010) |
| Blockers | 0 |
| Majors | 0 |
| Minors | 4 (all new in round 2; none carried over) |

## Prior-finding resolution

| Prior ID | Prior severity | Resolution | Status | Evidence |
|---|---|---|---|---|
| F-1 | major | Completeness seam added per DD-5 (`kFaultCodeCount`, `fault_code_table()`, `kStatusCodeCount`). New cases check table size, each design enumerator present once, `to_string` equals entry name, and the `StatusCode` count. Iteration cases now use the real table. Residual limit (no C++17 reflection) is stated by the design and cited in the Notes. | Resolved | TC-SENS-011.11 to .13 (real table), .16, .17, .18; TC-SENS-010.8, .14; design 5.3, 5.4, 7 |
| F-2 | major | `FakeClock::advance` and `set` return `Status` (DD-6). Expected values are exact: `Status{Ok, NONE}`, `Status{OutOfRange, NONE}`, `Status{InvalidArgument, TIME_NON_MONOTONIC}`; time unchanged after rejection. Every row of the design 5.2 table has a case. | Resolved | TC-SENS-011.2 to .6; TC-SENS-009.10, .11, .13, .14 |
| F-3 | major | Stub-clock case removed (former 009.8). No case needs a clock injection seam (DD-7). | Resolved | TC-SENS-009 has no .8; remaining `elapsed_us` cases take timestamps only. See F-14 for a residual Mocks entry on 009.9 |
| F-4 | minor | Out-of-range `FaultCode` value is `0xFFFF` (reserved category 0xFF), also asserted absent from the table (DD-8). | Resolved | TC-SENS-010.10; design 5.3 |
| F-5 | minor | `Result.value_or` failure case relabelled from `boundary` to `invalid`. | Resolved | TC-SENS-010.6 |
| F-6 | minor | Argument lists use `,` inside quoted cells; `;` is used only for multi-value fields and clause separation. | Resolved | TC-SENS-010.3, .5; TC-SENS-009.12 |
| F-7 | minor | Case is self-contained: all 13 value/name pairs and the category byte rule are listed and match design 5.3. | Resolved | TC-SENS-011.9 |
| F-8 | minor | NFR-009 removed from the value-type case; Notes explain verification is TC-CI-010 plus review (DD-8). | Resolved | TC-SENS-010.4; deferred row TC-CI-010 |
| F-9 | minor | FR-071 no longer cited by any case; Notes state adapters are verified with the adapters (DD-8). | Resolved | TC-SENS-011.7 |
| F-10 | minor | NFR-001 is cited by structural `noexcept` cases in all three TCs, matching design section 6. | Resolved | TC-SENS-011.19, TC-SENS-009.12, TC-SENS-010.13 |

## Check results

| # | Check | Result | Note |
|---|---|---|---|
| 1 | CSV validity | pass | Header equals the 21-column format in `csv-format.md`. Column count verified by inspection on every row (the validator script was not run). Case IDs and Test Names are unique. Multi-value cells use `;`. Case IDs are not contiguous (TC-SENS-009 skips .8 after the removal); the format does not require contiguity. |
| 2 | Coverage | pass | Every design section 7 TC is present: TC-SENS-011, 009, 010 at unit level; TC-CI-010 deferred. FR-011, NFR-013 and NFR-001 map to unit cases; NFR-009 maps to deferred TC-CI-010 (design 7, DD-8). New IDs 009, 010, CI-010 are flagged `new` and are next free in the area. Every design 5.2 table row, 5.3 rule and 5.4 rule has a case. |
| 3 | Case quality | pass | Each TC has nominal, boundary and invalid cases. Expected values are exact and observable. Minors F-11 (missing symbolic bounds), F-12, F-13. |
| 4 | Safety | pass | Design 5.7 and 7: ST-005 has no actuator, inhibit, failsafe, stale-sensor or malformed-input path; `Safety Path` is correctly empty. Own failure modes have fault-injection cases: backward clock (011.6, 009.4, 009.6, 009.7), saturating overflow (009.11, 009.13), out-of-range enums (010.9, 010.10), failed results (010.5, 010.6, 010.7); no default-constructed success (010.12). Every `fault` type case has `Fault Injection=Y`. |
| 5 | Behavior and isolation | pass | All cases use public interfaces; none asserts private state. No external dependency exists (value types, pure function, test-only `FakeClock`); `Mocks` is `none` except 009.9 (F-14, minor). No real time, I/O, threads or allocation. The design has no clock-consuming unit in ST-005 (DD-7), so no seam is missing. |
| 6 | TBD handling | pass | Symbolic or relational values throughout; OD-005 (011.3, 009.2), OD-012 (011.19, 009.12, 010.13), OD-014 (011.9) cited. Numeric values (13, 9, 0xFFFF, 0xFF, category bytes, enumerator values) trace to design 5.3 and 5.4; the rest are type limits. |
| 7 | Automation readiness | pass | Framework, Test Target, Test File and Test Name set on all 50 unit rows. `dfcs_unit_tests` exists in `tests/unit/CMakeLists.txt`; `test_fake_clock.cpp`, `test_fault_code.cpp`, `test_monotonic_clock.cpp`, `test_status.cpp` and `fake_clock.hpp` are creatable and must be added to that target. |

## Findings

| ID | Severity | Case ID | Source | Finding | Required fix |
|---|---|---|---|---|---|
| F-11 | minor | TC-SENS-009.3, TC-SENS-009.4 | Design 5.2 (`elapsed_us`); case-derivation (precondition clarity) | Preconditions give `later = earlier.value + 1` (009.3) and `earlier = later.value + 1` (009.4) with no bound on the symbolic base. At the maximum uint64 value the sum wraps and the expected result no longer holds. 009.9 and 009.11 state their bounds. | State that the symbolic base is below the maximum uint64 value. |
| F-12 | minor | TC-SENS-011.14 | Design 5.3 (names are "upper-case ASCII with underscores") | The case asserts upper-case letters and underscores only, which is stricter than the design wording; a later owner-story name containing a digit would fail it. The Notes record the design gap. | Have the design state whether digits are allowed, then align Expected; until then keep the Notes entry. |
| F-13 | minor | TC-SENS-011.19 | Design 6 (NFR-001 structural evidence) | One case in `test_fake_clock.cpp` under suite `FakeClock` asserts `noexcept` for `to_string(FaultCode)` and `fault_code_table()` as well as the `FakeClock` members. The `FaultCode` part sits outside its natural file and suite, and the case mixes two units. | Split into a `FaultCode` case in `test_fault_code.cpp` and a `FakeClock` case in `test_fake_clock.cpp`; the new Case ID is flagged `new`. |
| F-14 | minor | TC-SENS-009.9 | Design 5.2, DD-7 ("no case uses a clock stub"); csv-format (`Mocks`) | `Mocks` lists `MonotonicClock(fake)`, but `elapsed_us` takes `TimestampUs` values and consumes no clock; `FakeClock` is the unit under test in other cases. The entry reads as a mocked dependency that does not exist. | Set `Mocks` to `none` and keep the FakeClock note in Notes. |

## Coverage matrix

| FR/NFR | TC IDs | Case IDs |
|---|---|---|
| FR-011 | TC-SENS-011, TC-SENS-009 | 011.1 to 011.7, 011.20, 011.21; 009.1 to 009.7, 009.9 to 009.16 (009.12 also NFR-001) |
| NFR-013 | TC-SENS-011, TC-SENS-010 | 011.8 to 011.18; 010.1 to 010.14 |
| NFR-009 | TC-CI-010 (deferred; plus review) | Deferred row, no unit case (design 7, DD-8) |
| NFR-001 (structural) | TC-SENS-011, TC-SENS-009, TC-SENS-010 | 011.19; 009.12; 010.13 |
| NFR-005 (related) | TC-SENS-010 | 010.11 (by construction only) |
| NFR-008 (related) | TC-SENS-010 | 010.11 (include ban enforced by TC-CI-004, ST-004) |
| FR-071 (related) | none in ST-005 | Not cited, per design DD-8 |
