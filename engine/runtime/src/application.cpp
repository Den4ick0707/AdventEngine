#include <advent/core/fixed_timestep.hpp>
#include <advent/core/frame_clock.hpp>
#include <advent/core/log.hpp>
#include <advent/runtime/application.hpp>

#include <chrono>
#include <ranges>
#include <thread>

namespace advent::runtime
{
Application::Application(ApplicationConfig config)
    : m_config(std::move(config))
    , m_window(m_config.window)
    , m_vulkan(m_window, m_config.vulkan)
{}

Application::~Application() = default;

int Application::run()
{
    log::info("Application started");

    on_init();
    for (const auto& module : m_modules) {
        module->on_init();
    }

    FrameClock clock;
    FixedTimestep fixed(m_config.fixed_timestep_seconds);

    using SteadyClock = std::chrono::steady_clock;
    const auto frame_budget =
        m_config.max_fps > 0
            ? std::chrono::duration_cast<SteadyClock::duration>(std::chrono::duration<double>(1.0 / m_config.max_fps))
            : SteadyClock::duration::zero();

    std::uint64_t frames = 0;
    while (m_running && !m_window.should_close()) {
        const SteadyClock::time_point frame_start = SteadyClock::now();
        m_window.poll_events();

        if (m_window.is_minimized()) {
            m_window.wait_events();
            clock.reset(); // do not count the time spent minimized
            continue;
        }

        const double delta = clock.tick(frame_start);
        const auto delta_f = static_cast<float>(delta);

        const std::uint32_t steps = fixed.advance(delta);
        for (std::uint32_t i = 0; i < steps; ++i) {
            on_fixed_update(static_cast<float>(fixed.step()));
        }

        on_update(delta_f);
        for (const auto& module : m_modules) {
            module->on_update(delta_f);
        }

        on_render(static_cast<float>(fixed.alpha()));

        ++frames;
        if (m_config.max_frames > 0 && frames >= m_config.max_frames) {
            quit();
        }

        if (frame_budget > SteadyClock::duration::zero()) {
            std::this_thread::sleep_until(frame_start + frame_budget);
        }
    }

    m_vulkan.wait_idle();

    for (const auto& module : std::views::reverse(m_modules)) {
        module->on_shutdown();
    }
    on_shutdown();

    log::info("Application finished after {} frames", frames);
    return EXIT_SUCCESS;
}

void Application::quit() noexcept
{
    m_running = false;
}

platform::Window& Application::window() noexcept
{
    return m_window;
}

renderer::VulkanContext& Application::vulkan() noexcept
{
    return m_vulkan;
}

const ApplicationConfig& Application::config() const noexcept
{
    return m_config;
}

void Application::push_module(std::unique_ptr<Module> module)
{
    if (module) {
        m_modules.push_back(std::move(module));
    }
}
} // namespace advent::runtime
