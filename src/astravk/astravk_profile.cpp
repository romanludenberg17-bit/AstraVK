#include "astravk_profile.h"

#include <cstring>

namespace astravk {

namespace {

bool contains(const char* text, const char* needle) {
  return text && needle && std::strstr(text, needle) != nullptr;
}

} // namespace

DeviceProfile detectProfile(
    const VkPhysicalDeviceProperties& properties,
    const char* driverName,
    const char* driverInfo) {
  DeviceProfile result;

  // Vulkan does not expose the host Snapdragon CPU model. AstraVK therefore
  // targets the Adreno GPU family and treats the Snapdragon range as a
  // validation matrix rather than guessing the CPU from unrelated strings.
  result.qualcomm = properties.vendorID == 0x5143;
  result.turnip = contains(driverName, "Turnip")
               || contains(driverName, "turnip")
               || contains(driverInfo, "Turnip")
               || contains(driverInfo, "turnip");

  if (!result.qualcomm)
    return result;

  const char* name = properties.deviceName;

  // Adreno 725 is used by Snapdragon 7+ Gen 2 and is our lower target edge.
  if (contains(name, "Adreno 725") || contains(name, "Adreno 7")) {
    result.gpuFamily = GpuFamily::AdrenoA7xx;
    result.profile = result.turnip ? Profile::TurnipA7xx : Profile::Generic;
    result.preferredSubmitBatch = 4;
    result.pipelineWorkerHint = 2;
  }

  // Adreno 8xx covers the Snapdragon 8 Elite family, including the
  // Snapdragon 8 Elite Gen 5 / Adreno 840 validation target.
  if (contains(name, "Adreno 8")) {
    result.gpuFamily = GpuFamily::AdrenoA8xx;
    result.profile = result.turnip ? Profile::TurnipA8xx : Profile::Generic;
    result.preferredSubmitBatch = 8;
    result.pipelineWorkerHint = 3;
  }

  return result;
}

const char* gpuFamilyName(GpuFamily family) {
  switch (family) {
    case GpuFamily::AdrenoA7xx: return "Adreno A7xx";
    case GpuFamily::AdrenoA8xx: return "Adreno A8xx";
    default:                    return "Unknown";
  }
}

const char* profileName(Profile profile) {
  switch (profile) {
    case Profile::TurnipA7xx:    return "turnip-a7xx";
    case Profile::TurnipA8xx:    return "turnip-a8xx";
    case Profile::Compatibility: return "compatibility";
    default:                     return "generic";
  }
}

} // namespace astravk
