#pragma once

#include <cstdint>
#include <format>
#include <functional>
#include <string_view>
#include <utility>

namespace advent::log
{
enum class Level : std::uint8_t
{
    trace,
    debug,
    info,
    warn,
    error,
};

[[nodiscard]] std::string_view to_string(Level level) noexcept;

/// Messages below this level are dropped. Default: debug in Debug builds, info otherwise.
void set_min_level(Level level) noexcept;
[[nodiscard]] Level min_level() noexcept;
[[nodiscard]] bool is_enabled(Level level) noexcept;

/// Receives every message that passed the level filter. Pass an empty function to restore
/// the default sink (stdout for trace..info, stderr for warn..error).
using Sink = std::function<void(Level level, std::string_view message)>;
void set_sink(Sink sink);

/// Thread-safe. Prefer the formatting helpers below.
void write(Level level, std::string_view message);

template <typename... Args>
void trace(std::format_string<Args...> fmt, Args&&... args)
{
    if (is_enabled(Level::trace)) {
        write(Level::trace, std::format(fmt, std::forward<Args>(args)...));
    }
}

template <typename... Args>
void debug(std::format_string<Args...> fmt, Args&&... args)
{
    if (is_enabled(Level::debug)) {
        write(Level::debug, std::format(fmt, std::forward<Args>(args)...));
    }
}

template <typename... Args>
void info(std::format_string<Args...> fmt, Args&&... args)
{
    if (is_enabled(Level::info)) {
        write(Level::info, std::format(fmt, std::forward<Args>(args)...));
    }
}

template <typename... Args>
void warn(std::format_string<Args...> fmt, Args&&... args)
{
    if (is_enabled(Level::warn)) {
        write(Level::warn, std::format(fmt, std::forward<Args>(args)...));
    }
}

template <typename... Args>
void error(std::format_string<Args...> fmt, Args&&... args)
{
    if (is_enabled(Level::error)) {
        write(Level::error, std::format(fmt, std::forward<Args>(args)...));
    }
}
} // namespace advent::log
