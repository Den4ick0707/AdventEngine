#include <advent/core/assert.hpp>
#include <advent/core/log.hpp>
#include <advent/platform/window.hpp>

#include <GLFW/glfw3.h>

#include <span>
#include <stdexcept>

namespace advent::platform
{
namespace
{
int g_window_count = 0;

void on_glfw_error(int code, const char* description)
{
    log::error("GLFW error {}: {}", code, description != nullptr ? description : "(no description)");
}

void acquire_glfw()
{
    if (g_window_count == 0) {
        glfwSetErrorCallback(on_glfw_error);
        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        if (glfwVulkanSupported() != GLFW_TRUE) {
            glfwTerminate();
            throw std::runtime_error("Vulkan loader not found. Install a GPU driver with Vulkan support.");
        }
        log::debug("GLFW {} initialized", glfwGetVersionString());
    }
    ++g_window_count;
}

void release_glfw() noexcept
{
    --g_window_count;
    if (g_window_count == 0) {
        glfwTerminate();
    }
}
} // namespace

Window::Window(const WindowDesc& desc)
{
    ADVENT_ASSERT(desc.width > 0 && desc.height > 0, "window size must be non-zero");

    acquire_glfw();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);

    m_window = glfwCreateWindow(static_cast<int>(desc.width), static_cast<int>(desc.height), desc.title.c_str(),
                                nullptr, nullptr);
    if (m_window == nullptr) {
        release_glfw();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, &Window::on_framebuffer_resize);

    log::info("Window created: \"{}\" {}x{}", desc.title, desc.width, desc.height);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    release_glfw();
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::wait_events()
{
    glfwWaitEvents();
}

bool Window::should_close() const
{
    return glfwWindowShouldClose(m_window) == GLFW_TRUE;
}

void Window::request_close()
{
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Window::set_title(const std::string& title)
{
    glfwSetWindowTitle(m_window, title.c_str());
}

Extent Window::framebuffer_extent() const
{
    int width  = 0;
    int height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    return {.width = static_cast<std::uint32_t>(width), .height = static_cast<std::uint32_t>(height)};
}

bool Window::is_minimized() const
{
    const Extent extent = framebuffer_extent();
    return extent.width == 0 || extent.height == 0;
}

bool Window::consume_resize() noexcept
{
    const bool resized = m_resized;
    m_resized          = false;
    return resized;
}

std::vector<const char*> Window::required_vulkan_extensions() const
{
    std::uint32_t count     = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    if (extensions == nullptr) {
        throw std::runtime_error("GLFW cannot create Vulkan surfaces on this system");
    }
    const std::span<const char*> view(extensions, count);
    return {view.begin(), view.end()};
}

GLFWwindow* Window::native_handle() const noexcept
{
    return m_window;
}

void Window::on_framebuffer_resize(GLFWwindow* window, int /*width*/, int /*height*/)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self != nullptr) {
        self->m_resized = true;
    }
}
} // namespace advent::platform
