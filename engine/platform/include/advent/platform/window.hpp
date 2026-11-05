#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct GLFWwindow;

namespace advent::platform
{
struct WindowDesc
{
    std::string title    = "AdventEngine";
    std::uint32_t width  = 1280;
    std::uint32_t height = 720;
    bool resizable       = true;
};

struct Extent
{
    std::uint32_t width  = 0;
    std::uint32_t height = 0;
};

/// OS window backed by GLFW. Created without an OpenGL context (GLFW_NO_API) for Vulkan.
/// GLFW is initialized with the first window and terminated with the last one.
/// Must be created and used on the main thread.
class Window
{
public:
    explicit Window(const WindowDesc& desc = {});
    ~Window();

    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&)                 = delete;
    Window& operator=(Window&&)      = delete;

    void poll_events();

    /// Blocks until at least one event arrives. Use while the window is minimized.
    void wait_events();

    [[nodiscard]] bool should_close() const;
    void request_close();

    void set_title(const std::string& title);

    /// Size in pixels of the drawable area (what the swapchain must match).
    [[nodiscard]] Extent framebuffer_extent() const;
    [[nodiscard]] bool is_minimized() const;

    /// Returns true once after the framebuffer size changed: recreate the swapchain then.
    [[nodiscard]] bool consume_resize() noexcept;

    /// Instance extensions required to create a Vulkan surface for this window.
    [[nodiscard]] std::vector<const char*> required_vulkan_extensions() const;

    [[nodiscard]] GLFWwindow* native_handle() const noexcept;

private:
    static void on_framebuffer_resize(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window = nullptr;
    bool m_resized       = false;
};
} // namespace advent::platform
