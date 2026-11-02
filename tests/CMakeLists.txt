# Unit tests. Each test is a small executable registered in CTest.
# A test framework (doctest / Catch2) is still an open decision - see CLAUDE.md;
# until then tests use the minimal helpers from test_support.hpp.

function(advent_add_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE advent::core)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    advent_set_warnings(${name})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

advent_add_test(core_fixed_timestep_test core/fixed_timestep_test.cpp)
advent_add_test(core_frame_clock_test core/frame_clock_test.cpp)
advent_add_test(core_log_test core/log_test.cpp)
