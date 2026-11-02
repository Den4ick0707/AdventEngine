#include <advent/core/frame_clock.hpp>

#include <test_support.hpp>

#include <chrono>

namespace
{
using namespace std::chrono_literals;
using Clock = advent::FrameClock::Clock;

constexpr double k_eps = 1e-9;

void first_tick_returns_zero()
{
    advent::FrameClock clock;
    ADVENT_CHECK_NEAR(clock.tick(Clock::time_point{} + 5s), 0.0, k_eps);
}

void returns_time_between_ticks()
{
    advent::FrameClock clock;
    const Clock::time_point start{};
    clock.tick(start);
    ADVENT_CHECK_NEAR(clock.tick(start + 16ms), 0.016, k_eps);
    ADVENT_CHECK_NEAR(clock.tick(start + 50ms), 0.034, k_eps);
    ADVENT_CHECK(clock.frame_count() == 3);
}

void clamps_long_frames()
{
    advent::FrameClock clock(0.1);
    const Clock::time_point start{};
    clock.tick(start);
    ADVENT_CHECK_NEAR(clock.tick(start + 3s), 0.1, k_eps);
}

void reset_skips_the_gap()
{
    advent::FrameClock clock;
    const Clock::time_point start{};
    clock.tick(start);
    clock.reset();
    ADVENT_CHECK_NEAR(clock.tick(start + 10s), 0.0, k_eps);
    ADVENT_CHECK_NEAR(clock.tick(start + 10s + 20ms), 0.02, k_eps);
}
} // namespace

int main()
{
    first_tick_returns_zero();
    returns_time_between_ticks();
    clamps_long_frames();
    reset_skips_the_gap();
    return advent::test::finish();
}
