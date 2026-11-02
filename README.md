# Drone Flight Control System (DFCS)

Proof-of-concept Quad-X flight-control firmware for STM32H743, developed simulation-first. Not certified, production-ready or flight-ready (NFR-017). See `docs/Drone_Flight_Control_System_SRS.md`.

## Host build and tests (ST-001)

Required tools: CMake, Ninja, a C++17 compiler (GCC on the reference host), Git and network access to fetch GoogleTest. Pinned versions live in `tools/tool-versions.cmake`; configure warns on a mismatch.

```text
cmake --preset host
cmake --build --preset host
ctest --preset host
```

CI should use `host-strict`, which fails configure on a tool version mismatch:

```text
cmake --preset host-strict
cmake --build --preset host-strict
ctest --preset host-strict
```

Build output goes to `build/` (git-ignored).
