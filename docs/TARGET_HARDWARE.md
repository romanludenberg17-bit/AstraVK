# AstraVK target hardware

AstraVK is tuned for Qualcomm Snapdragon phones from the Snapdragon 7 Gen 2-era performance class through Snapdragon 8 Elite Gen 5.

The implementation targets the GPU/driver stack rather than trying to infer a CPU model from Vulkan. The validation matrix is therefore organized around Adreno generations and Turnip behaviour.

## Primary range

| Snapdragon range | Adreno class | AstraVK profile |
|---|---|---|
| 7 Gen 2-era / 7-series midrange | A7xx | `turnip-a7xx` |
| 7+ Gen 2 and newer high-performance 7-series | A7xx | `turnip-a7xx` |
| 8 Gen 2 / 8 Gen 3 class | A7xx | `turnip-a7xx` |
| Snapdragon 8 Elite | A8xx | `turnip-a8xx` |
| Snapdragon 8 Elite Gen 5 | A8xx / A8x Slice family | `turnip-a8xx` |

The exact GPU identity must always be read from Vulkan properties. Device names are used only as a compatibility fallback because driver-side GPU IDs can differ between Turnip branches.

## Optimization priorities

1. CPU submission overhead under Wine/Box64.
2. Vulkan command-buffer and queue-submit churn.
3. Descriptor update/allocation pressure.
4. Pipeline creation and cache hit rate.
5. Short-lived allocation pressure and memory fragmentation.
6. Frame-time spikes rather than average FPS alone.
7. Turnip-specific behaviour only behind GPU/driver feature detection.
8. Thermal stability: avoid optimizations that require forced clocks or unsafe synchronization.

## Important architecture rule

In Winlator/GameHub, the game CPU code may be translated by Box64/Box86 while DXVK is still a Windows DLL. AstraVK therefore focuses on reducing the amount of CPU work and synchronization performed by DXVK and on improving its Vulkan interaction with Turnip. It does not assume that compiling the DXVK DLL itself for ARM64 is the correct optimization.

## Validation tiers

- **Tier A:** Adreno A7xx + Turnip, starting from the lower target range.
- **Tier B:** faster A7xx Snapdragon devices, including flagship-class A7xx GPUs.
- **Tier C:** Adreno A8xx / Snapdragon 8 Elite.
- **Tier D:** Snapdragon 8 Elite Gen 5 / newest A8x hardware.

Every optimization must be benchmarked on at least one device from its affected tier and retain a compatibility fallback.
