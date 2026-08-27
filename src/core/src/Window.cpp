#include "Window.hpp"
#include "GlfwWindow.hpp"

namespace AdventEngine::Core
{
std::unique_ptr<Window> Window::Create(const WindowProps& props)
{
    return std::make_unique<GlfwWindow>(props);
}
}