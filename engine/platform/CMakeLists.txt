add_library(advent_platform STATIC)
add_library(advent::platform ALIAS advent_platform)

target_sources(advent_platform
    PRIVATE
        src/window.cpp
)

target_include_directories(advent_platform
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# GLFW stays PRIVATE: the public header only forward-declares GLFWwindow.
target_link_libraries(advent_platform
    PUBLIC  advent::core
    PRIVATE glfw
)
target_compile_definitions(advent_platform PRIVATE GLFW_INCLUDE_NONE)
advent_set_warnings(advent_platform)
