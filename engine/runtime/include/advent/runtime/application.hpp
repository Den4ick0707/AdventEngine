#pragma once

#include <advent/core/log.hpp>
#include <advent/platform/window.hpp>
#include <advent/renderer/vulkan_context.hpp>
#include <advent/runtime/module.hpp>

#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <memory>
#include <utility>
#include <vector>

namespace advent::runtime
{
struct ApplicationConfig
{
    platform::WindowDesc window{};
    renderer::VulkanContextDesc vulkan{};

    double fixed_timestep_seconds = 1.0 / 60.0; // rate of on_fixed_update()
    std::uint32_t max_fps         = 0;          // 0 = unlimited (later: limited by vsync)
    std::uint64_t max_frames      = 0;          // 0 = run until the window closes; used by smoke tests
};

// Base class for games and tools. Owns the window and the Vulkan context and runs the main loop:
//
//   poll events -> on_fixed_update() x N -> on_update() -> modules -> on_render(alpha)
//
// on_fixed_update() runs at a constant rate (physics, gameplay simulation);
// on_update() runs once per frame with the variable frame time (camera, UI);
// alpha in on_render() is how far we are between two fixed steps (for interpolation).
class Application
{
public:
    explicit Application(ApplicationConfig config = {});
    virtual ~Application();

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&)                 = delete;
    Application& operator=(Application&&)      = delete;

    // Runs the main loop until the window closes or quit() is called. Returns the process exit code.
    int run();
    void quit() noexcept;

    [[nodiscard]] platform::Window& window() noexcept;
    [[nodiscard]] renderer::VulkanContext& vulkan() noexcept;
    [[nodiscard]] const ApplicationConfig& config() const noexcept;

protected:
    void push_module(std::unique_ptr<Module> module);

    virtual void on_init() {}
    virtual void on_fixed_update(float /*step_seconds*/) {}
    virtual void on_update(float /*delta_seconds*/) {}
    virtual void on_render(float /*alpha*/) {}
    virtual void on_shutdown() {}

private:
    ApplicationConfig m_config;
    platform::Window m_window;        // declared before m_vulkan: the surface needs the window
    renderer::VulkanContext m_vulkan; // destroyed before the window
    std::vector<std::unique_ptr<Module>> m_modules;
    bool m_running = true;
};

// Creates App, runs it and turns any escaping exception into a logged error and a non-zero exit code.
//
//   int main() { return advent::runtime::run_application<MyGame>(config); }
template <std::derived_from<Application> App, typename... Args>
int run_application(Args&&... args) noexcept
{
    try {
        App app(std::forward<Args>(args)...);
        return app.run();
    } catch (const std::exception& error) {
        log::error("Fatal error: {}", error.what());
    } catch (...) {
        log::error("Fatal error: unknown exception");
    }
    return EXIT_FAILURE;
}
} // namespace advent::runtime
