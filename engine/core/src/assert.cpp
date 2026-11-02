#include <advent/core/assert.hpp>
#include <advent/core/log.hpp>

#include <cstdlib>

namespace advent::detail
{
void assert_failed(const char* expression, const char* message, std::source_location location)
{
    log::error("Assertion failed: {}\n  message:  {}\n  location: {}:{} ({})", expression, message,
               location.file_name(), location.line(), location.function_name());
    std::abort();
}
} // namespace advent::detail
