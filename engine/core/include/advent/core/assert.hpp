#pragma once

#include <source_location>

namespace advent::detail
{
// Logs the failed condition with its location and aborts (a debugger stops here).
[[noreturn]] void assert_failed(const char* expression, const char* message, std::source_location location);
} // namespace advent::detail

// ADVENT_ASSERT(condition, "message") checks programmer errors in Debug builds.
// In Release the condition is not evaluated. Do not put side effects into it.
#ifdef ADVENT_ENABLE_ASSERTS
#define ADVENT_ASSERT(condition, message)                                                                              \
    do {                                                                                                               \
        if (!static_cast<bool>(condition)) [[unlikely]] {                                                              \
            ::advent::detail::assert_failed(#condition, message, std::source_location::current());                     \
        }                                                                                                              \
    } while (false)
#else
#define ADVENT_ASSERT(condition, message)                                                                              \
    do {                                                                                                               \
        (void)sizeof(condition);                                                                                       \
    } while (false)
#endif
