#include <advent/core/assert.hpp>
#include <advent/core/fixed_timestep.hpp>

#include <cmath>

namespace advent
{
FixedTimestep::FixedTimestep(double step_seconds, std::uint32_t max_steps_per_frame) noexcept
    : m_step(step_seconds)
    , m_max_steps(max_steps_per_frame)
{
    ADVENT_ASSERT(step_seconds > 0.0, "fixed step must be positive");
    ADVENT_ASSERT(max_steps_per_frame > 0, "at least one step per frame is required");
}

std::uint32_t FixedTimestep::advance(double frame_seconds) noexcept
{
    if (frame_seconds > 0.0) {
        m_accumulator += frame_seconds;
    }

    std::uint32_t steps = 0;
    while (m_accumulator >= m_step && steps < m_max_steps) {
        m_accumulator -= m_step;
        ++steps;
    }

    // Too far behind: drop whole steps we could not run, keep the fractional part.
    if (m_accumulator >= m_step) {
        m_accumulator = std::fmod(m_accumulator, m_step);
    }

    return steps;
}

double FixedTimestep::step() const noexcept
{
    return m_step;
}

double FixedTimestep::alpha() const noexcept
{
    return m_accumulator / m_step;
}
} // namespace advent
