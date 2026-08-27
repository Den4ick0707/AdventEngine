#ifndef __ADVENTENGINE_CORE_APPLICATION_HPP__
#define __ADVENTENGINE_CORE_APPLICATION_HPP__

#include "IModule.hpp"
#include "Window.hpp"

namespace AdventEngine::Core
{
class Application
{
public:
    Application();
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void Run();
    void Quit() noexcept;

    [[nodiscard]]
    Window& GetWindow() noexcept;

protected:
    void PushModule(std::unique_ptr<IModule> module);

    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Render() {}
    virtual void Shutdown() {}

private:
    std::unique_ptr<Window> m_Window;
    std::vector<std::unique_ptr<IModule>> m_Modules;

    bool m_Running = true;
};

std::unique_ptr<Application> CreateApplication();
}

#endif