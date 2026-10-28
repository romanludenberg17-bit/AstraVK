# Allocator memory diagnostics: implementation plan

Status: design note only. This repository does not yet contain an imported DXVK source tree, so this document is not a runtime patch.

## Baseline selection

Use upstream DXVK commit `9c5551cbefa8a6782d1b94554e254a2ff483e254` as the initial source-inspection baseline for allocator diagnostics. This is a pinned source revision, not a claim that AstraVK already contains it or that it is the newest upstream release. Before importing code, record the selected release/tag and commit in the repository and preserve all upstream notices and license terms.

Relevant upstream files at that revision:
- `src/dxvk/dxvk_memory.h`
- `src/dxvk/dxvk_memory.cpp`
- `src/dxvk/hud/dxvk_hud_item.cpp`
- `dxvk.conf`

The upstream allocator already defines `DxvkMemoryStats` with allocated, used, and budget fields, plus detailed per-memory-type and per-chunk statistics. Therefore, first inspect and reuse existing counters and HUD plumbing rather than adding a parallel allocator accounting system.

## Proposed diagnostic additions

After source import and a clean baseline build, add an opt-in diagnostic mode that records:

- current and peak allocator-backed bytes allocated;
- current and peak bytes used by suballocations;
- current and peak chunk count/capacity;
- per-memory-type totals, where available;
- allocation/free event counts and Vulkan allocation failures;
- Vulkan heap budget/usage when the relevant extension is available;
- timestamp and a process-local session identifier.

Do not log every allocation by default. Prefer periodic snapshots and allocation-failure events; make verbose per-event logging opt-in to avoid excessive I/O, lock contention, and log growth.

## Correctness constraints

- Keep statistics updates synchronized with the allocator's existing synchronization model.
- Track allocator-owned bytes separately from driver-reported heap usage; they are different measurements.
- Do not label total Android RAM or process PSS as DXVK memory.
- Do not force-free chunks that still contain live allocations or are referenced by pending GPU work.
- Do not add a memory cap or change allocation strategy in the diagnostics-only commit.
- Keep diagnostics disabled by default and preserve upstream behavior when disabled.

## Commit sequence

1. Import/pin upstream DXVK and make an unmodified baseline build pass.
2. Verify existing HUD/statistics and allocator accounting.
3. Add opt-in peak counters and snapshot logging only.
4. Add tests or debug assertions for counter consistency and repeated allocate/free cycles.
5. Benchmark the same game scene before and after; record DXVK allocator totals alongside process PSS and Android memory pressure.
6. Only then make a separate allocator-policy change, with its own benchmark and rollback path.

## Interpretation

DXVK's allocator statistics cover DXVK-managed Vulkan allocations; they do not include every allocation made by the game, Wine, Box64/FEX, the Vulkan driver, Android, or other applications. A system-wide memory percentage cannot be attributed to DXVK without process and allocator measurements.
