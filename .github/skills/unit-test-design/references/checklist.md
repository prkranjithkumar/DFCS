# Completion checklist

Run [Test-UnitTestSpec.ps1](../scripts/Test-UnitTestSpec.ps1) first; it automates items marked (script). Verify the rest by reading the CSV. Report pass/fail per item.

1. Every design section 6 TC is covered, as unit cases or deferred with level (script)
2. Every FR/NFR in the design section 2.1 table maps to >=1 case (script)
3. TC IDs unchanged from design; new IDs flagged `new` and next free (script flags; next-free is manual)
4. Every design safety path has a fault-injection row, Fault Injection=Y and Safety Path set (script checks the design's Fault injection column; manual for the rest)
5. CSV valid: constant column count, quoting, header, enums, patterns, unique Case IDs and Test Names (script)
6. Each TC has nominal, boundary and invalid cases (script)
7. No invented numbers; all thresholds `TBD (OD-xxx)` with Open Decisions set (manual)
8. Every case asserts observable unit behavior via the public interface; none depends on internals (manual)
9. Every external dependency of each case is listed in `Mocks`; no case needs a real dependency (manual; script warns on malformed Mocks entries)
10. Expected results exact and observable (script warns on vague phrases)
11. Only `docs/test-specs/<STORY-ID>-unit-tests.csv` changed (manual)
