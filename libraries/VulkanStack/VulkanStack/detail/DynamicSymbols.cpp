#include <unordered_map>

#include <vulkan/vulkan.hpp>

struct VkDynamicSymbols {
  PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = nullptr;
  PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
};

// Lol locking.
static std::unordered_map<VkInstance, VkDynamicSymbols> vkInstanceSymbolsMap;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
  VkInstance instance,
  const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
  const VkAllocationCallbacks* pAllocator,
  VkDebugReportCallbackEXT* pCallback) {
  auto& statics = vkInstanceSymbolsMap[instance];

  if (!statics.vkCreateDebugReportCallbackEXT) {
    statics.vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugReportCallbackEXT");
  }

  if (!statics.vkCreateDebugReportCallbackEXT) {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  return statics.vkCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
  VkInstance instance,
  VkDebugReportCallbackEXT callback,
  const VkAllocationCallbacks* pAllocator) {
  auto& statics = vkInstanceSymbolsMap[instance];

  if (!statics.vkDestroyDebugReportCallbackEXT) {
    statics.vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugReportCallbackEXT");
  }

  if (!statics.vkDestroyDebugReportCallbackEXT) {
    return;
  }

  return statics.vkDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}