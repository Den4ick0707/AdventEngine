#include <advent/core/log.hpp>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <mutex>
#include <string>

namespace advent::log
{
namespace
{
#ifdef NDEBUG
constexpr Level k_default_level = Level::info;
#else
constexpr Level k_default_level = Level::debug;
#endif

std::atomic<Level> g_min_level{k_default_level};

struct LogState
{
    std::mutex mutex;
    Sink sink;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
};

LogState& state()
{
    static LogState instance;
    return instance;
}

void default_sink(Level level, std::string_view message, double seconds)
{
    std::FILE* stream      = level >= Level::warn ? stderr : stdout;
    const std::string line = std::format("[{:9.3f}] [{:<5}] {}\n", seconds, to_string(level), message);
    std::fputs(line.c_str(), stream);
    std::fflush(stream);
}
} // namespace

std::string_view to_string(Level level) noexcept
{
    switch (level) {
    case Level::trace:
        return "trace";
    case Level::debug:
        return "debug";
    case Level::info:
        return "info";
    case Level::warn:
        return "warn";
    case Level::error:
        return "error";
    }
    return "?";
}

void set_min_level(Level level) noexcept
{
    g_min_level.store(level, std::memory_order_relaxed);
}

Level min_level() noexcept
{
    return g_min_level.load(std::memory_order_relaxed);
}

bool is_enabled(Level level) noexcept
{
    return level >= min_level();
}

void set_sink(Sink sink)
{
    LogState& s = state();
    const std::scoped_lock lock(s.mutex);
    s.sink = std::move(sink);
}

void write(Level level, std::string_view message)
{
    if (!is_enabled(level)) {
        return;
    }

    LogState& s = state();
    const std::scoped_lock lock(s.mutex);
    if (s.sink) {
        s.sink(level, message);
        return;
    }

    const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - s.start;
    default_sink(level, message, elapsed.count());
}
} // namespace advent::log
