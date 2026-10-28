# AstraVK low-memory strategy (8 GB Android devices)

## Goal

Reduce DXVK-attributable peak memory and allocation spikes on Android devices with 8 GB physical RAM, especially during long game loads and transitions in demanding games such as Fallout 4 and Grand Theft Auto V under Winlator/GameHub.

This is a target, not a claim that DXVK alone can make every game fit into 8 GB. Total pressure includes the game, Wine, Box64, the container/runtime, Vulkan driver, GPU allocations, Android, and background applications.

## First principle: measure before trimming

Every test should record, where the runtime exposes it:

- Android total/free memory and memory-pressure or low-memory-kill events.
- Per-process PSS/RSS for the container and game processes.
- Vulkan heap usage/budget when VK_EXT_memory_budget is available.
- DXVK resource allocation totals, peak bytes, allocation count, and lifetime.
- Pipeline cache size and shader/pipeline compilation events.
- Game, save, graphics settings, resolution, Turnip build, DXVK build, Box64 preset, and runtime version.
- Load-stage timestamps and whether the process crashed, was killed by Android, or failed with a game/DXVK error.

Compare baseline and patch on the same device, scene, settings, runtime and driver. Track peak memory and frame-time percentiles, not average FPS alone.

## Prioritized engineering work

### P0 — resource lifetime and peak accounting

1. Add opt-in diagnostics for DXVK-owned GPU resources and staging allocations.
2. Audit resource lifetime across load screens, level transitions, swapchain recreation and device teardown.
3. Ensure temporary upload/readback buffers and transient allocations are released or recycled when their last use is complete.
4. Check for duplicate copies of large resources during upload and format conversion.
5. Avoid retaining resources solely because a cache entry exists when the resource can be safely evicted.

No resource may be destroyed or reused until GPU use has completed according to Vulkan synchronization rules.

### P1 — memory-budget-aware allocation

1. Query VK_EXT_memory_budget when available; otherwise use conservative fallback accounting.
2. Introduce a budget policy based on reported device-local heap budget and DXVK's own tracked allocations, not a hard-coded percentage of Android RAM.
3. Apply soft limits to reclaimable caches and temporary pools before allocation pressure becomes critical.
4. Prefer bounded pools and reuse for frequently recreated staging resources, while preventing unbounded pool growth.
5. Make budget policy configurable and disable it by default until validation demonstrates correctness.

Vulkan heap budget is not the same as Android free RAM. It must not be treated as a complete view of system memory.

### P2 — caches

- Bound reclaimable pipeline/state caches by configurable memory and age limits.
- Preserve useful on-disk shader/pipeline caches where supported; do not delete them on every launch.
- Separate cache eviction from live-resource destruction.
- Measure cold-start compilation stutter and warm-run memory separately.

### P3 — mobile presets

Proposed profiles:

- `memory-safe`: conservative cache and transient-pool budgets; intended for 8 GB devices.
- `balanced`: default policy after validation.
- `performance`: larger cache allowance when the device budget permits.
- `compatibility`: disable experimental allocation policies.

Profiles must never bypass Vulkan synchronization or game correctness requirements.

## Explicit non-goals

- Do not promise that 8 GB is sufficient for every game or configuration.
- Do not silently lower a game's texture quality or resolution from DXVK.
- Do not destroy live GPU resources to force memory under a target.
- Do not assume the Vulkan process owns all memory reported by Android.
- Do not add an unbounded background shader compiler or cache.
- Do not claim improvement without reproducible before/after measurements.

## Initial validation matrix

For each device, record exact SoC, GPU, Android version, RAM, Turnip build, Winlator/GameHub version and configuration.

1. 8 GB device, Adreno A7xx, low-memory pressure.
2. 8 GB device, Adreno A7xx, heavy load/level transition.
3. 8 GB device, Adreno A8xx where available.
4. Compare 8 GB and 12 GB devices when possible to distinguish a true leak/peak-allocation problem from a platform capacity limit.

Required outcomes for a patch:

- Lower or equal peak DXVK-attributable memory in the target workload.
- No new rendering corruption or device loss.
- No increase in crash rate.
- Report cold and warm runs separately.
- Include the raw logs or a reproducible test recipe.
