Module: hardware abstraction layer

- STM32CubeH7 HAL/LL is confined to `hal/` and `drivers/`. Never expose HAL types in headers consumed by the portable core (NFR-008).
- DMA buffers and Cortex-M7 D-cache coherency are handled explicitly: document memory placement, alignment and cache maintenance (NFR-007, OD-004).
- Board revision and pin/peripheral map are TBD (OD-002, OD-003); verify against board documentation.
