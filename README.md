# AstraVK

A mobile-focused DXVK research and optimization project for ARM64 phones, Vulkan/Turnip, Winlator and GameHub.

AstraVK is based on the upstream DXVK architecture and is intended to keep upstream compatibility while experimenting with mobile-specific performance improvements. The project does **not** promise a universal FPS increase; changes are benchmark-driven and must preserve correctness.

## Goals

- Snapdragon/Adreno + Mesa Turnip focused tuning
- ARM64/Box64/Wine workload optimization
- lower CPU submission overhead where measurable
- reduce frame-time spikes and shader/pipeline stutter
- mobile-aware pipeline/cache behavior
- conservative device detection and profiles
- reproducible Winlator/GameHub builds
- automated benchmark and regression tracking

## Planned profiles

- `auto` — conservative automatic tuning
- `turnip` — Adreno/Turnip-oriented defaults
- `performance` — latency/frame-time oriented
- `compatibility` — minimal behavioral changes

## Status

Early development. The repository currently contains the project specification and integration scaffolding; optimized DXVK source changes will be introduced incrementally and benchmarked against upstream.

## Upstream

AstraVK is based on [DXVK](https://github.com/doitsujin/dxvk), a Vulkan-based implementation of D3D8/9/10/11 for Wine/Linux.

Do not redistribute proprietary game files, drivers, firmware, or copyrighted game assets with AstraVK.

## Benchmark policy

Every performance change should include:

1. the affected DXVK subsystem;
2. a reproducible workload;
3. average FPS and 1%/0.1% low frame time where possible;
4. CPU/GPU utilization observations;
5. shader-compilation/stutter observations;
6. compatibility results on at least one Turnip device.

AstraVK aims to be faster **without turning correctness into an optional feature**.