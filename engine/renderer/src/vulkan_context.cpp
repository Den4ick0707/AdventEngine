#include <advent/core/log.hpp>
#include <advent/platform/window.hpp>
#include <advent/renderer/vulkan_check.hpp>
#include <advent/renderer/vulkan_context.hpp>

// After volk.h (via vulkan_context.hpp), so GLFW declares glfwCreateWindowSurface.
#include <GLFW/glfw3.h>

#include <array>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace advent::renderer
{
namespace
{
constexpr const char* k_validation_layer = "VK_LAYER_KHRONOS_validation";
constexpr std::array k_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

std::string version_string(std::uint32_t version)
{
    return std::format("{}.{}.{}", VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version),
                       VK_API_VERSION_PATCH(version));
}

std::string_view device_type_name(VkPhysicalDeviceType type) noexcept
{
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "discrete";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "integrated";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "virtual";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "cpu";
    default:
        return "other";
    }
}

int device_type_score(VkPhysicalDeviceType type) noexcept
{
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return 1000;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return 100;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return 50;
    default:
        return 10;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL on_debug_message(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                VkDebugUtilsMessageTypeFlagsEXT /*types*/,
                                                const VkDebugUtilsMessengerCallbackDataEXT* data, void* /*user_data*/)
{
    const char* message = (data != nullptr && data->pMessage != nullptr) ? data->pMessage : "(no message)";

    if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) {
        log::error("[vulkan] {}", message);
    } else if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0) {
        log::warn("[vulkan] {}", message);
    } else {
        log::trace("[vulkan] {}", message);
    }
    return VK_FALSE; // never abort the call that triggered the message
}

VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info()
{
    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = on_debug_message;
    return info;
}

bool has_instance_layer(std::string_view name)
{
    std::uint32_t count = 0;
    ADVENT_VK_CHECK(vkEnumerateInstanceLayerProperties(&count, nullptr));
    std::vector<VkLayerProperties> layers(count);
    ADVENT_VK_CHECK(vkEnumerateInstanceLayerProperties(&count, layers.data()));

    for (const VkLayerProperties& layer : layers) {
        if (name == layer.layerName) {
            return true;
        }
    }
    return false;
}

bool has_instance_extension(std::string_view name)
{
    std::uint32_t count = 0;
    ADVENT_VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
    std::vector<VkExtensionProperties> extensions(count);
    ADVENT_VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data()));

    for (const VkExtensionProperties& extension : extensions) {
        if (name == extension.extensionName) {
            return true;
        }
    }
    return false;
}

bool has_device_extensions(VkPhysicalDevice device)
{
    std::uint32_t count = 0;
    ADVENT_VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr));
    std::vector<VkExtensionProperties> available(count);
    ADVENT_VK_CHECK(vkEnumerateDeviceExtensionProperties(device, nullptr, &count, available.data()));

    for (const char* required : k_device_extensions) {
        bool found = false;
        for (const VkExtensionProperties& extension : available) {
            if (std::string_view(required) == extension.extensionName) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

bool has_required_features(VkPhysicalDevice device)
{
    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

    VkPhysicalDeviceFeatures2 features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &features13;
    vkGetPhysicalDeviceFeatures2(device, &features);

    return features13.dynamicRendering == VK_TRUE && features13.synchronization2 == VK_TRUE;
}

bool can_present(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    std::uint32_t format_count = 0;
    ADVENT_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr));
    std::uint32_t mode_count = 0;
    ADVENT_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count, nullptr));
    return format_count > 0 && mode_count > 0;
}

std::optional<QueueFamilies> find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    std::uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

    QueueFamilies result;
    for (std::uint32_t index = 0; index < count; ++index) {
        const bool graphics = (families[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;

        VkBool32 present = VK_FALSE;
        ADVENT_VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &present));

        // Prefer one family that can do both: no ownership transfers between queues.
        if (graphics && present == VK_TRUE) {
            return QueueFamilies{index, index};
        }
        if (graphics && result.graphics == UINT32_MAX) {
            result.graphics = index;
        }
        if (present == VK_TRUE && result.present == UINT32_MAX) {
            result.present = index;
        }
    }

    if (result.graphics == UINT32_MAX || result.present == UINT32_MAX) {
        return std::nullopt;
    }
    return result;
}

// Returns an empty string if the device is usable, otherwise the reason it is not.
std::string check_device(VkPhysicalDevice device, const VkPhysicalDeviceProperties& properties, VkSurfaceKHR surface)
{
    if (properties.apiVersion < VK_API_VERSION_1_3) {
        return std::format("supports only Vulkan {}", version_string(properties.apiVersion));
    }
    if (!has_device_extensions(device)) {
        return "no VK_KHR_swapchain";
    }
    if (!has_required_features(device)) {
        return "no dynamicRendering / synchronization2";
    }
    if (!find_queue_families(device, surface)) {
        return "no graphics or present queue";
    }
    if (!can_present(device, surface)) {
        return "cannot present to this window";
    }
    return {};
}
} // namespace

VulkanContext::VulkanContext(platform::Window& window, const VulkanContextDesc& desc)
{
    try {
        create_instance(window, desc);
        create_debug_messenger();
        create_surface(window);
        pick_physical_device();
        create_device();
        create_allocator();
    } catch (...) {
        destroy();
        throw;
    }
}

VulkanContext::~VulkanContext()
{
    if (m_device != VK_NULL_HANDLE) {
        // Nothing useful can be done with an error during shutdown.
        static_cast<void>(vkDeviceWaitIdle(m_device));
    }
    destroy();
    log::debug("Vulkan context destroyed");
}

void VulkanContext::create_instance(const platform::Window& window, const VulkanContextDesc& desc)
{
    if (volkInitialize() != VK_SUCCESS) {
        throw std::runtime_error("Vulkan loader not found. Install a GPU driver with Vulkan support.");
    }

    const std::uint32_t loader_version = volkGetInstanceVersion();
    if (loader_version < VK_API_VERSION_1_3) {
        throw std::runtime_error(
            std::format("Vulkan 1.3 is required, but the installed loader supports {}. Update the GPU driver.",
                        version_string(loader_version)));
    }

    std::vector<const char*> extensions = window.required_vulkan_extensions();
    std::vector<const char*> layers;

    if (desc.enable_validation) {
        if (has_instance_layer(k_validation_layer) && has_instance_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            layers.push_back(k_validation_layer);
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            m_validation_enabled = true;
        } else {
            log::warn("Validation requested, but {} is not installed (install the Vulkan SDK). "
                      "Continuing without validation.",
                      k_validation_layer);
        }
    }

    VkApplicationInfo app_info{};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName   = desc.app_name;
    app_info.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
    app_info.pEngineName        = "AdventEngine";
    app_info.engineVersion      = VK_MAKE_API_VERSION(0, 0, 1, 0);
    app_info.apiVersion         = VK_API_VERSION_1_3;

    VkInstanceCreateInfo info{};
    info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo        = &app_info;
    info.enabledLayerCount       = static_cast<std::uint32_t>(layers.size());
    info.ppEnabledLayerNames     = layers.data();
    info.enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();

    // Chained messenger also reports problems inside vkCreateInstance / vkDestroyInstance.
    const VkDebugUtilsMessengerCreateInfoEXT messenger_info = debug_messenger_info();
    if (m_validation_enabled) {
        info.pNext = &messenger_info;
    }

    ADVENT_VK_CHECK(vkCreateInstance(&info, nullptr, &m_instance));
    volkLoadInstanceOnly(m_instance);

    log::info("Vulkan instance created (loader {}, validation {})", version_string(loader_version),
              m_validation_enabled ? "on" : "off");
}

void VulkanContext::create_debug_messenger()
{
    if (!m_validation_enabled) {
        return;
    }
    const VkDebugUtilsMessengerCreateInfoEXT info = debug_messenger_info();
    ADVENT_VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_instance, &info, nullptr, &m_debug_messenger));
}

void VulkanContext::create_surface(platform::Window& window)
{
    ADVENT_VK_CHECK(glfwCreateWindowSurface(m_instance, window.native_handle(), nullptr, &m_surface));
}

void VulkanContext::pick_physical_device()
{
    std::uint32_t count = 0;
    ADVENT_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &count, nullptr));
    if (count == 0) {
        throw std::runtime_error("No GPU with Vulkan support found");
    }
    std::vector<VkPhysicalDevice> devices(count);
    ADVENT_VK_CHECK(vkEnumeratePhysicalDevices(m_instance, &count, devices.data()));

    int best_score = -1;
    for (VkPhysicalDevice device : devices) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device, &properties);
        const std::string_view name = properties.deviceName;

        const std::string problem = check_device(device, properties, m_surface);
        if (!problem.empty()) {
            log::debug("GPU skipped: {} ({})", name, problem);
            continue;
        }

        const int score = device_type_score(properties.deviceType);
        log::debug("GPU candidate: {} ({}, score {})", name, device_type_name(properties.deviceType), score);
        if (score > best_score) {
            best_score        = score;
            m_physical_device = device;
            m_properties      = properties;
        }
    }

    if (m_physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("No GPU supports Vulkan 1.3 with dynamic rendering, synchronization2 "
                                 "and presentation to this window. Update the GPU driver.");
    }

    m_queue_families = *find_queue_families(m_physical_device, m_surface);

    VkPhysicalDeviceFeatures features{};
    vkGetPhysicalDeviceFeatures(m_physical_device, &features);
    m_anisotropy_supported = features.samplerAnisotropy == VK_TRUE;

    log::info("GPU: {} ({}, Vulkan {}, driver {})", std::string_view(m_properties.deviceName),
              device_type_name(m_properties.deviceType), version_string(m_properties.apiVersion),
              m_properties.driverVersion);
}

void VulkanContext::create_device()
{
    const float priority = 1.0F;

    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    const std::array families = {m_queue_families.graphics, m_queue_families.present};
    for (const std::uint32_t family : families) {
        if (!queue_infos.empty() && queue_infos.front().queueFamilyIndex == family) {
            continue; // graphics and present share one family
        }
        VkDeviceQueueCreateInfo queue_info{};
        queue_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = family;
        queue_info.queueCount       = 1;
        queue_info.pQueuePriorities = &priority;
        queue_infos.push_back(queue_info);
    }

    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;
    features13.synchronization2 = VK_TRUE;

    VkPhysicalDeviceFeatures2 features{};
    features.sType                      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext                      = &features13;
    features.features.samplerAnisotropy = m_anisotropy_supported ? VK_TRUE : VK_FALSE;

    VkDeviceCreateInfo info{};
    info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext                   = &features;
    info.queueCreateInfoCount    = static_cast<std::uint32_t>(queue_infos.size());
    info.pQueueCreateInfos       = queue_infos.data();
    info.enabledExtensionCount   = static_cast<std::uint32_t>(k_device_extensions.size());
    info.ppEnabledExtensionNames = k_device_extensions.data();

    ADVENT_VK_CHECK(vkCreateDevice(m_physical_device, &info, nullptr, &m_device));
    volkLoadDevice(m_device);

    vkGetDeviceQueue(m_device, m_queue_families.graphics, 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_queue_families.present, 0, &m_present_queue);

    set_debug_name(VK_OBJECT_TYPE_QUEUE, reinterpret_cast<std::uint64_t>(m_graphics_queue), "graphics queue");
}

void VulkanContext::create_allocator()
{
    VmaAllocatorCreateInfo info{};
    info.vulkanApiVersion = VK_API_VERSION_1_3;
    info.instance         = m_instance;
    info.physicalDevice   = m_physical_device;
    info.device           = m_device;

    VmaVulkanFunctions functions{};
    ADVENT_VK_CHECK(vmaImportVulkanFunctionsFromVolk(&info, &functions));
    info.pVulkanFunctions = &functions;

    ADVENT_VK_CHECK(vmaCreateAllocator(&info, &m_allocator));
}

void VulkanContext::destroy() noexcept
{
    if (m_allocator != VK_NULL_HANDLE) {
        vmaDestroyAllocator(m_allocator);
        m_allocator = VK_NULL_HANDLE;
    }
    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
    if (m_debug_messenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        m_debug_messenger = VK_NULL_HANDLE;
    }
    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

void VulkanContext::set_debug_name(VkObjectType type, std::uint64_t handle, const char* name) const
{
    if (!m_validation_enabled || m_device == VK_NULL_HANDLE) {
        return;
    }
    VkDebugUtilsObjectNameInfoEXT info{};
    info.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.objectType   = type;
    info.objectHandle = handle;
    info.pObjectName  = name;
    ADVENT_VK_CHECK(vkSetDebugUtilsObjectNameEXT(m_device, &info));
}

void VulkanContext::wait_idle() const
{
    ADVENT_VK_CHECK(vkDeviceWaitIdle(m_device));
}
} // namespace advent::renderer
