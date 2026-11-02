#pragma once

// volk.h must be included before vk_mem_alloc.h (see vma_impl.cpp).
#include <volk.h>

#include <vk_mem_alloc.h>

#include <cstdint>

namespace advent::platform
{
class Window;
}

namespace advent::renderer
{
inline constexpr bool k_validation_default = ADVENT_VALIDATION_DEFAULT != 0;

struct VulkanContextDesc
{
    const char* app_name   = "AdventEngine";
    bool enable_validation = k_validation_default;
};

struct QueueFamilies
{
    std::uint32_t graphics = UINT32_MAX;
    std::uint32_t present  = UINT32_MAX;
};

// Owns the core Vulkan objects shared by the whole renderer:
// instance, debug messenger, surface, physical/logical device, queues and the VMA allocator.
//
// Requires Vulkan 1.3 with dynamicRendering and synchronization2.
// Construction throws (VulkanError / std::runtime_error) if no suitable GPU is found.
class VulkanContext
{
public:
    explicit VulkanContext(platform::Window& window, const VulkanContextDesc& desc = {});
    ~VulkanContext();

    VulkanContext(const VulkanContext&)            = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;
    VulkanContext(VulkanContext&&)                 = delete;
    VulkanContext& operator=(VulkanContext&&)      = delete;

    [[nodiscard]] VkInstance instance() const noexcept { return m_instance; }
    [[nodiscard]] VkSurfaceKHR surface() const noexcept { return m_surface; }
    [[nodiscard]] VkPhysicalDevice physical_device() const noexcept { return m_physical_device; }
    [[nodiscard]] VkDevice device() const noexcept { return m_device; }
    [[nodiscard]] VmaAllocator allocator() const noexcept { return m_allocator; }

    [[nodiscard]] const QueueFamilies& queue_families() const noexcept { return m_queue_families; }
    [[nodiscard]] VkQueue graphics_queue() const noexcept { return m_graphics_queue; }
    [[nodiscard]] VkQueue present_queue() const noexcept { return m_present_queue; }

    [[nodiscard]] const VkPhysicalDeviceProperties& device_properties() const noexcept { return m_properties; }
    [[nodiscard]] bool validation_enabled() const noexcept { return m_validation_enabled; }

    // Gives a Vulkan object a readable name for validation messages and RenderDoc.
    // Does nothing when validation is disabled.
    void set_debug_name(VkObjectType type, std::uint64_t handle, const char* name) const;

    void wait_idle() const;

private:
    void create_instance(const platform::Window& window, const VulkanContextDesc& desc);
    void create_debug_messenger();
    void create_surface(platform::Window& window);
    void pick_physical_device();
    void create_device();
    void create_allocator();
    void destroy() noexcept;

    VkInstance m_instance                      = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface                     = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device         = VK_NULL_HANDLE;
    VkDevice m_device                          = VK_NULL_HANDLE;
    VmaAllocator m_allocator                   = VK_NULL_HANDLE;

    QueueFamilies m_queue_families{};
    VkQueue m_graphics_queue = VK_NULL_HANDLE;
    VkQueue m_present_queue  = VK_NULL_HANDLE;

    VkPhysicalDeviceProperties m_properties{};
    bool m_validation_enabled   = false;
    bool m_anisotropy_supported = false;
};
} // namespace advent::renderer
