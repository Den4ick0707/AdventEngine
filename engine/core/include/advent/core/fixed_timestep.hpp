#pragma once

#include <cstdint>

namespace advent
{
/// Accumulates variable frame time and turns it into a whole number of fixed simulation steps.
///
/// ```cpp
/// const std::uint32_t steps = fixed.advance(frame_seconds);
/// for (std::uint32_t i = 0; i < steps; ++i) simulate(fixed.step());
/// render(fixed.alpha()); // interpolate between the previous and current simulation state
/// ```
///
/// At most max_steps_per_frame steps run per frame; leftover time beyond that is dropped
/// so a slow frame cannot trigger an ever-growing number of catch-up steps.
class FixedTimestep
{
public:
    explicit FixedTimestep(double step_seconds, std::uint32_t max_steps_per_frame = 8) noexcept;

    /// Adds frame time and returns how many fixed steps to run this frame.
    std::uint32_t advance(double frame_seconds) noexcept;

    [[nodiscard]] double step() const noexcept;

    /// Fraction of the next step already accumulated, in [0, 1).
    [[nodiscard]] double alpha() const noexcept;

private:
    double m_step;
    double m_accumulator = 0.0;
    std::uint32_t m_max_steps;
};
} // namespace advent
