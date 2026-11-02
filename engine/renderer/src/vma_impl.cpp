// Single translation unit with the Vulkan Memory Allocator implementation.
// volk.h must come first: VMA then exposes vmaImportVulkanFunctionsFromVolk().
#include <volk.h>

#define VMA_IMPLEMENTATION // NOLINT(readability-identifier-naming)
#include <vk_mem_alloc.h>
