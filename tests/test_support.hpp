#pragma once

// Minimal test helpers until a test framework is chosen (see CLAUDE.md, "Open decisions").
//
//   int main() {
//       ADVENT_CHECK(1 + 1 == 2);
//       ADVENT_CHECK_NEAR(0.1 + 0.2, 0.3, 1e-9);
//       return advent::test::finish();
//   }

#include <cmath>
#include <cstdio>

namespace advent::test
{
inline int& failure_count()
{
    static int count = 0;
    return count;
}

inline void check(bool passed, const char* expression, const char* file, int line)
{
    if (!passed) {
        std::fprintf(stderr, "%s:%d: CHECK failed: %s\n", file, line, expression);
        ++failure_count();
    }
}

inline int finish()
{
    if (failure_count() == 0) {
        std::puts("All checks passed");
        return 0;
    }
    std::fprintf(stderr, "%d check(s) failed\n", failure_count());
    return 1;
}
} // namespace advent::test

#define ADVENT_CHECK(expression) ::advent::test::check(static_cast<bool>(expression), #expression, __FILE__, __LINE__)

#define ADVENT_CHECK_NEAR(actual, expected, tolerance)                                                                 \
    ::advent::test::check(std::abs((actual) - (expected)) <= (tolerance), #actual " ~= " #expected, __FILE__, __LINE__)
