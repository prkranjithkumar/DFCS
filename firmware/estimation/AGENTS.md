Module: state estimation

- Use only validated, monotonic-timestamped samples (FR-011, FR-012).
- Every output carries validity and health (FR-021). Missing, stale or invalid mandatory input marks the output invalid (FR-022).
- Estimator parameters come from configuration, not code (FR-023).
- The algorithm is undecided (OD-006); do not pick one silently. Propose it and validate it in simulation first.
- Tests: `TC-EST-NNN` for validity and fault behavior; include stale and out-of-range fault injection.
