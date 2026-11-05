#pragma once

#include <chrono>
#include <cstdint>

namespace advent
{
/// Measures time between frames.
///
/// The delta is clamped (0.25 s by default) so a breakpoint or a window drag does not
/// produce one huge step that breaks the simulation.
class FrameClock
{
public:
    using Clock = std::chrono::steady_clock;

    explicit FrameClock(double max_delta_seconds = 0.25) noexcept;

    /// Seconds since the previous tick. The first tick after construction or reset() returns 0.
    double tick() noexcept;
    double tick(Clock::time_point now) noexcept;

    /// Forget the previous time point, e.g. after the window was minimized.
    void reset() noexcept;

    [[nodiscard]] std::uint64_t frame_count() const noexcept;

private:
    Clock::time_point m_last;
    double m_max_delta;
    std::uint64_t m_frame_count = 0;
    bool m_has_last             = false;
};
} // namespace advent
