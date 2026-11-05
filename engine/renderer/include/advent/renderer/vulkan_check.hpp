#pragma once

#include <volk.h>

#include <source_location>
#include <stdexcept>

namespace advent::renderer
{
/// Thrown when a Vulkan call fails. Carries the VkResult for callers that want to react.
class VulkanError : public std::runtime_error
{
public:
    VulkanError(VkResult result, const char* expression, std::source_location location);

    [[nodiscard]] VkResult result() const noexcept;

private:
    VkResult m_result;
};

[[nodiscard]] const char* to_string(VkResult result) noexcept;
} // namespace advent::renderer

/// Checks a VkResult-returning call and throws VulkanError on failure (negative codes).
/// Do not use it for vkAcquireNextImageKHR / vkQueuePresentKHR: VK_ERROR_OUT_OF_DATE_KHR and
/// VK_SUBOPTIMAL_KHR there mean "recreate the swapchain", not "fail".
#define ADVENT_VK_CHECK(expression)                                                                                    \
    do {                                                                                                               \
        const VkResult advent_vk_result_ = (expression);                                                               \
        if (advent_vk_result_ < 0) [[unlikely]] {                                                                      \
            throw ::advent::renderer::VulkanError(advent_vk_result_, #expression, std::source_location::current());    \
        }                                                                                                              \
    } while (false)
