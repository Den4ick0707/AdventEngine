add_library(advent_core STATIC)
add_library(advent::core ALIAS advent_core)

target_sources(advent_core
    PRIVATE
        src/assert.cpp
        src/fixed_timestep.cpp
        src/frame_clock.cpp
        src/log.cpp
)

target_include_directories(advent_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(advent_core PUBLIC advent::glm)
target_compile_features(advent_core PUBLIC cxx_std_20)
target_compile_definitions(advent_core PUBLIC $<$<CONFIG:Debug>:ADVENT_ENABLE_ASSERTS>)
advent_set_warnings(advent_core)
