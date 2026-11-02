Module: FreeRTOS platform layer and task wiring

- FreeRTOS headers stay out of the portable core; only platform code such as this module includes them (NFR-008).
- Task priorities, periods, deadlines and stack sizes are TBD until detailed design (NFR-002, OD-005); do not invent them.
- Create RTOS objects at init only; no allocation afterwards (NFR-005).
- Data shared between tasks and interrupts needs documented synchronization and ownership (NFR-006).
- Wire task-health reporting to the watchdog policy owned by `firmware/safety/` (FR-064). Measure and report critical-task execution time (NFR-003).
