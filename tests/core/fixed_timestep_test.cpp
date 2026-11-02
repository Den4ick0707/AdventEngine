#include <advent/core/fixed_timestep.hpp>

#include <test_support.hpp>

namespace
{
constexpr double k_step = 1.0 / 60.0;
constexpr double k_eps  = 1e-9;

void runs_no_step_before_enough_time_accumulates()
{
    advent::FixedTimestep fixed(k_step);
    ADVENT_CHECK(fixed.advance(k_step * 0.5) == 0);
    ADVENT_CHECK_NEAR(fixed.alpha(), 0.5, k_eps);
}

void runs_one_step_per_step_duration()
{
    advent::FixedTimestep fixed(k_step);
    ADVENT_CHECK(fixed.advance(k_step) == 1);
    ADVENT_CHECK_NEAR(fixed.alpha(), 0.0, k_eps);
}

void carries_remainder_between_frames()
{
    advent::FixedTimestep fixed(k_step);
    ADVENT_CHECK(fixed.advance(k_step * 0.75) == 0);
    ADVENT_CHECK(fixed.advance(k_step * 0.75) == 1); // 1.5 steps accumulated
    ADVENT_CHECK_NEAR(fixed.alpha(), 0.5, k_eps);
}

void caps_steps_and_drops_excess_time()
{
    advent::FixedTimestep fixed(k_step, 4);
    ADVENT_CHECK(fixed.advance(k_step * 10.25) == 4);
    ADVENT_CHECK(fixed.alpha() < 1.0);
    ADVENT_CHECK(fixed.advance(0.0) == 0); // the dropped time does not come back
}

void ignores_negative_frame_time()
{
    advent::FixedTimestep fixed(k_step);
    ADVENT_CHECK(fixed.advance(-1.0) == 0);
    ADVENT_CHECK_NEAR(fixed.alpha(), 0.0, k_eps);
}
} // namespace

int main()
{
    runs_no_step_before_enough_time_accumulates();
    runs_one_step_per_step_duration();
    carries_remainder_between_frames();
    caps_steps_and_drops_excess_time();
    ignores_negative_frame_time();
    return advent::test::finish();
}
