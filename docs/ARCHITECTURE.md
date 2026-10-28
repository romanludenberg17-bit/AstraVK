# AstraVK architecture

## Layering

AstraVK keeps the upstream DXVK D3D8/9/10/11 translation model and concentrates mobile-specific work in narrowly scoped subsystems rather than maintaining a large collection of game hacks.

Target stack:

`Windows game -> Wine/Proton -> DXVK/AstraVK -> Vulkan -> Mesa Turnip -> Adreno`

On Android containers, Box64/Box86 and the container runtime remain outside AstraVK. AstraVK should avoid duplicating work that belongs in Wine, Box64 or Mesa.

## Optimization priorities

### P0 — frame-time stability

- pipeline/shader cache behavior
- avoid redundant synchronization
- reduce avoidable command-buffer churn
- reduce allocations and short-lived objects on hot paths

### P1 — CPU overhead

- submission batching where semantics allow
- descriptor/update path profiling
- lock contention and worker-thread behavior
- ARM64-friendly hot-path data structures

### P2 — Turnip interaction

- detect Adreno/Turnip capabilities through Vulkan properties/features
- prefer standard Vulkan paths that map efficiently to Turnip
- keep vendor workarounds isolated and version-gated

### P3 — mobile constraints

- thermal-aware presets should be external/configurable rather than forcing clocks
- avoid unnecessary memory pressure
- make cache locations predictable for container environments

## Non-goals

- game-specific cheats or anti-cheat bypasses
- proprietary driver modifications
- unsafe synchronization shortcuts that trade correctness for benchmark numbers
- claiming FPS gains without measurements

## Compatibility rule

Every optimization should have a switch or a clear rollback path during development. Upstream behavior remains the baseline.