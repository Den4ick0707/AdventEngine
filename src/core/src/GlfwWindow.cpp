#include "GlfwWindow.hpp"

#include <stdexcept>

namespace AdventEngine::Core
{
GlfwWindow::GlfwWindow(const WindowProps& props)
    : m_Props(props)
{
    Initialize(props);
}

GlfwWindow::~GlfwWindow()
{
    Shutdown();
}

void GlfwWindow::Initialize(const WindowProps& props)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(
        static_cast<int>(props.width),
        static_cast<int>(props.height),
        props.title.c_str(),
        nullptr,
        nullptr
    );

    if (!m_Window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
}

void GlfwWindow::Shutdown() noexcept
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    glfwTerminate();
}

void GlfwWindow::PollEvents()
{
    glfwPollEvents();
}

bool GlfwWindow::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

std::uint32_t GlfwWindow::GetWidth() const
{
    return m_Props.width;
}

std::uint32_t GlfwWindow::GetHeight() const
{
    return m_Props.height;
}

GLFWwindow* GlfwWindow::GetNativeWindow() const noexcept
{
    return m_Window;
}
}