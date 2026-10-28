#pragma once

#include <cstdint>

#include <vulkan/vulkan_core.h>

namespace astravk {

enum class GpuFamily : uint8_t {
  Unknown,
  AdrenoA7xx,
  AdrenoA8xx,
};

enum class Profile : uint8_t {
  Generic,
  TurnipA7xx,
  TurnipA8xx,
  Compatibility,
};

struct DeviceProfile {
  GpuFamily gpuFamily = GpuFamily::Unknown;
  Profile profile = Profile::Generic;
  bool qualcomm = false;
  bool turnip = false;
  bool unifiedMemory = false;

  // Policy hints only. A subsystem must explicitly opt into them and provide
  // a benchmark/rollback path before changing DXVK behaviour.
  uint32_t preferredSubmitBatch = 1;
  uint32_t pipelineWorkerHint = 0;
};

// Vulkan exposes GPU identity through VkPhysicalDeviceProperties. Driver
// strings are supplied separately from VkPhysicalDeviceDriverProperties.
DeviceProfile detectProfile(
  const VkPhysicalDeviceProperties& properties,
  const char* driverName = nullptr,
  const char* driverInfo = nullptr);

const char* gpuFamilyName(GpuFamily family);
const char* profileName(Profile profile);

} // namespace astravk
