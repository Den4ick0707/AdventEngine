#ifndef __ADVENTENGINE_CORE_GLFW_WINDOW_HPP__
#define __ADVENTENGINE_CORE_GLFW_WINDOW_HPP__

#include "Window.hpp"

#include <GLFW/glfw3.h>

namespace AdventEngine::Core
{
class GlfwWindow final : public Window
{
public:
    explicit GlfwWindow(const WindowProps& props);
    ~GlfwWindow() override;

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;

    void PollEvents() override;

    [[nodiscard]]
    bool ShouldClose() const override;

    [[nodiscard]]
    std::uint32_t GetWidth() const override;

    [[nodiscard]]
    std::uint32_t GetHeight() const override;

    [[nodiscard]]
    GLFWwindow* GetNativeWindow() const noexcept;

private:
    void Initialize(const WindowProps& props);
    void Shutdown() noexcept;

    GLFWwindow* m_Window = nullptr;

    WindowProps m_Props;
};
}

#endif