add_library(advent_renderer STATIC)
add_library(advent::renderer ALIAS advent_renderer)

target_sources(advent_renderer
    PRIVATE
        src/vma_impl.cpp
        src/vulkan_check.cpp
        src/vulkan_context.cpp
)

target_include_directories(advent_renderer
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Vulkan types (volk, VMA) are part of the renderer's public API; GLFW is only used to create the surface.
target_link_libraries(advent_renderer
    PUBLIC  advent::core advent::platform advent::volk advent::vma
    PRIVATE glfw
)

target_compile_definitions(advent_renderer
    PUBLIC  ADVENT_VALIDATION_DEFAULT=$<IF:$<BOOL:${ADVENT_ENABLE_VALIDATION}>,1,0>
    PRIVATE GLFW_INCLUDE_NONE
)
advent_set_warnings(advent_renderer)
