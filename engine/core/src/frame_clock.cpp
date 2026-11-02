#include <advent/core/assert.hpp>
#include <advent/core/frame_clock.hpp>

#include <algorithm>

namespace advent
{
FrameClock::FrameClock(double max_delta_seconds) noexcept
    : m_max_delta(max_delta_seconds)
{
    ADVENT_ASSERT(max_delta_seconds > 0.0, "max delta must be positive");
}

double FrameClock::tick() noexcept
{
    return tick(Clock::now());
}

double FrameClock::tick(Clock::time_point now) noexcept
{
    ++m_frame_count;

    if (!m_has_last) {
        m_last     = now;
        m_has_last = true;
        return 0.0;
    }

    const std::chrono::duration<double> elapsed = now - m_last;
    m_last                                      = now;
    return std::clamp(elapsed.count(), 0.0, m_max_delta);
}

void FrameClock::reset() noexcept
{
    m_has_last = false;
}

std::uint64_t FrameClock::frame_count() const noexcept
{
    return m_frame_count;
}
} // namespace advent
