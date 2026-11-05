// Sandbox: demo application used for manual testing and the CI smoke test.
//
//   sandbox [--frames N] [--max-fps N] [--no-validation] [--help]

#include <advent/core/log.hpp>
#include <advent/runtime/application.hpp>

#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <optional>
#include <span>
#include <string_view>
#include <system_error>
#include <utility>

namespace
{
constexpr std::string_view k_usage = "Usage: sandbox [options]\n"
                                     "  --frames N        exit after N frames (0 = until the window closes)\n"
                                     "  --max-fps N       frame rate limit (0 = unlimited, default 240)\n"
                                     "  --no-validation   disable Vulkan validation layers\n"
                                     "  --help            show this message\n";

constexpr std::uint32_t k_default_max_fps = 240; // until the swapchain presents with vsync (ADR 0004)

class Sandbox final : public advent::runtime::Application
{
public:
    using Application::Application;

protected:
    void on_init() override { advent::log::info("Sandbox initialized"); }

    void on_update(float delta_seconds) override
    {
        m_title_timer += delta_seconds;
        ++m_frames_since_title;
        if (m_title_timer >= 1.0F) {
            const float fps = static_cast<float>(m_frames_since_title) / m_title_timer;
            window().set_title(std::format("AdventEngine Sandbox | {:.0f} FPS", fps));
            m_title_timer        = 0.0F;
            m_frames_since_title = 0;
        }
    }

private:
    float m_title_timer                = 0.0F;
    std::uint32_t m_frames_since_title = 0;
};

template <typename T>
std::optional<T> parse_number(std::string_view text)
{
    T value{};
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        return std::nullopt;
    }
    return value;
}

// Returns the config, or nullopt when the program should exit (help or bad arguments).
std::optional<advent::runtime::ApplicationConfig> parse_args(std::span<char*> args, int& exit_code)
{
    advent::runtime::ApplicationConfig config;
    config.window.title = "AdventEngine Sandbox";
    config.max_fps      = k_default_max_fps;

    for (std::size_t i = 1; i < args.size(); ++i) {
        const std::string_view arg = args[i];
        const bool has_value       = i + 1 < args.size();

        if (arg == "--help" || arg == "-h") {
            std::fwrite(k_usage.data(), 1, k_usage.size(), stdout);
            exit_code = EXIT_SUCCESS;
            return std::nullopt;
        }
        if (arg == "--no-validation") {
            config.vulkan.enable_validation = false;
            continue;
        }
        if ((arg == "--frames" || arg == "--max-fps") && has_value) {
            const std::string_view value = args[++i];
            if (arg == "--frames") {
                const auto frames = parse_number<std::uint64_t>(value);
                if (frames) {
                    config.max_frames = *frames;
                    continue;
                }
            } else {
                const auto fps = parse_number<std::uint32_t>(value);
                if (fps) {
                    config.max_fps = *fps;
                    continue;
                }
            }
            advent::log::error("Invalid value for {}: '{}'", arg, value);
            exit_code = 2;
            return std::nullopt;
        }

        advent::log::error("Unknown or incomplete argument: '{}'\n{}", arg, k_usage);
        exit_code = 2;
        return std::nullopt;
    }
    return config;
}
} // namespace

int main(int argc, char** argv)
{
    int exit_code = EXIT_SUCCESS;
    auto config   = parse_args(std::span(argv, static_cast<std::size_t>(argc)), exit_code);
    if (!config) {
        return exit_code;
    }
    return advent::runtime::run_application<Sandbox>(std::move(*config));
}
