#include "Application.hpp"

#include <utility>

namespace AdventEngine::Core
{
Application::Application()
    : m_Window(Window::Create())
{
}

Application::~Application() = default;

void Application::Run()
{
    Initialize();

    for (auto& module : m_Modules)
    {
        module->Initialize();
    }

    while (m_Running && !m_Window->ShouldClose())
    {
        m_Window->PollEvents();

        // TODO: replace with actual frame delta time
        constexpr float deltaTime = 0.0f;

        Update(deltaTime);

        for (auto& module : m_Modules)
        {
            module->Update(deltaTime);
        }

        Render();
    }

    for (auto it = m_Modules.rbegin(); it != m_Modules.rend(); ++it)
    {
        (*it)->Shutdown();
    }

    Shutdown();
}

void Application::Quit() noexcept
{
    m_Running = false;
}

Window& Application::GetWindow() noexcept
{
    return *m_Window;
}

void Application::PushModule(std::unique_ptr<IModule> module)
{
    if (module)
    {
        m_Modules.emplace_back(std::move(module));
    }
}
}