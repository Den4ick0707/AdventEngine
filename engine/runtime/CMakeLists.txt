add_library(advent_runtime STATIC)
add_library(advent::runtime ALIAS advent_runtime)

target_sources(advent_runtime
    PRIVATE
        src/application.cpp
)

target_include_directories(advent_runtime
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(advent_runtime
    PUBLIC advent::core advent::platform advent::renderer
)
advent_set_warnings(advent_runtime)
