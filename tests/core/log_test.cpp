#include <advent/core/log.hpp>

#include <test_support.hpp>

#include <string>
#include <utility>
#include <vector>

namespace
{
std::vector<std::pair<advent::log::Level, std::string>> g_messages;

void capture(advent::log::Level level, std::string_view message)
{
    g_messages.emplace_back(level, std::string(message));
}

void formats_messages()
{
    g_messages.clear();
    advent::log::set_min_level(advent::log::Level::trace);
    advent::log::info("{} + {} = {}", 2, 2, 4);
    ADVENT_CHECK(g_messages.size() == 1);
    ADVENT_CHECK(g_messages.front().first == advent::log::Level::info);
    ADVENT_CHECK(g_messages.front().second == "2 + 2 = 4");
}

void filters_by_level()
{
    g_messages.clear();
    advent::log::set_min_level(advent::log::Level::warn);
    advent::log::debug("hidden");
    advent::log::info("hidden");
    advent::log::warn("shown");
    advent::log::error("shown");
    ADVENT_CHECK(g_messages.size() == 2);
    ADVENT_CHECK(!advent::log::is_enabled(advent::log::Level::info));
    ADVENT_CHECK(advent::log::is_enabled(advent::log::Level::error));
}

void level_names()
{
    ADVENT_CHECK(advent::log::to_string(advent::log::Level::warn) == "warn");
    ADVENT_CHECK(advent::log::to_string(advent::log::Level::error) == "error");
}
} // namespace

int main()
{
    advent::log::set_sink(capture);
    formats_messages();
    filters_by_level();
    level_names();
    advent::log::set_sink({});
    return advent::test::finish();
}
