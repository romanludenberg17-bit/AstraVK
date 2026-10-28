# Low-memory Android tuning (8 GB devices)

This guide targets DXVK-based Windows games running through Winlator/GameHub on Android with unified system memory. It is a diagnostic starting point, not a guarantee against out-of-memory crashes.

## Important: reported memory is not an allocation cap

On mobile GPUs, CPU and GPU commonly share physical DRAM. DXVK's `dxgi.maxDeviceMemory` and `dxgi.maxSharedMemory` override the memory sizes reported to the game through DXGI; they do **not** impose a hard limit on Vulkan allocations and do not directly reclaim RAM. Some games use the reported value to choose texture/pool sizes, others ignore it or respond differently.

Start with a per-game `dxvk.conf`:

```ini
# Reported memory values only; experiment per game.
dxgi.maxDeviceMemory = 2048
dxgi.maxSharedMemory = 1024

# Limit frames in flight / latency where supported.
dxgi.maxFrameLatency = 1

# Limit DXVK shader compiler worker threads; 0 means automatic/default.
dxvk.numCompilerThreads = 2
```

These are upstream DXVK option names in the referenced 2.x codebase. Check the exact DXVK version bundled by your Winlator/GameHub build before assuming every option is available.

### What not to assume

- `d3d11.maxFrameLatency` is not the upstream option name in the checked DXVK source; use `dxgi.maxFrameLatency`.
- `dxgi.numBackBuffers` is not a supported upstream DXVK option in the checked configuration/source. Do not add it expecting an effect.
- Lowering reported memory may reduce a game's chosen pools, but can also cause texture streaming, stutter, or game-specific regressions. Test one setting at a time. If needed, compare 1536 or 1024 MB for `dxgi.maxDeviceMemory`; these are experiments, not universal recommendations.
- Compiler thread count mainly trades shader-compilation throughput/CPU pressure against concurrent work. It is not a general-purpose large RAM reduction switch.

## A safe test procedure

1. Save the original config and record the exact DXVK version, game, emulator/runtime, graphics driver (e.g. Turnip version), resolution, and graphics settings.
2. Record Android memory pressure before launch, during the heaviest loading/transition, and immediately before a crash. If possible, capture the crash/logcat reason.
3. Test the baseline, then change only one setting at a time. Repeat the same scene/load sequence.
4. Compare peak process PSS/RSS, Android memory pressure, Vulkan allocation errors, load time, texture behavior, and crash outcome. A system-wide “93% RAM used” figure alone does not identify which component caused the failure.
5. Keep a setting only if repeatable tests show an improvement without unacceptable rendering or stability regressions.

## What AstraVK itself can improve

AstraVK currently contains configuration/documentation scaffolding, not an integrated DXVK runtime patch. Real allocator changes require importing the upstream source and instrumenting DXVK-owned allocations first. Priorities should be:

- account for allocation sizes, peaks, lifetimes, and allocation failures;
- identify oversized transient upload/readback allocations and duplicated resource copies;
- return genuinely empty allocator blocks only when safe with respect to Vulkan/GPU synchronization;
- bound only reclaimable caches/pools, with measured policies and a fallback to upstream behavior.

DXVK cannot control all memory used by the game, Wine, Box64/FEX, the Vulkan driver, Android, or other apps. Do not claim a fixed memory saving without before/after measurements on real devices.
