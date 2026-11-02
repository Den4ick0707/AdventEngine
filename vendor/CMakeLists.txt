# Third-party dependencies, vendored as plain copies (see docs/adr/0005 and vendor/README.md).
# All of them are added as SYSTEM so their headers never trigger project warnings.

# --- GLFW: window and input ----------------------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_INSTALL OFF CACHE INTERNAL "")
add_subdirectory(glfw SYSTEM EXCLUDE_FROM_ALL)

# --- GLM: math -----------------------------------------------------------------
# Own interface target instead of GLM's CMake: header-only, no compiled library,
# and no SIMD flags forced on every CPU.
add_library(advent_glm INTERFACE)
add_library(advent::glm ALIAS advent_glm)
target_include_directories(advent_glm SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/glm")
target_compile_definitions(advent_glm INTERFACE
    GLM_FORCE_DEPTH_ZERO_TO_ONE   # Vulkan depth range is [0, 1]
    GLM_FORCE_RADIANS
)

# --- Vulkan headers + volk loader ------------------------------------------------
# Headers come from the submodule, so the build does not depend on the SDK version.
# The Vulkan SDK is still needed at run time for validation layers and for glslc.
set(VULKAN_HEADERS_ENABLE_TESTS OFF CACHE INTERNAL "")
set(VULKAN_HEADERS_ENABLE_INSTALL OFF CACHE INTERNAL "")
add_subdirectory(Vulkan-Headers SYSTEM EXCLUDE_FROM_ALL)

add_library(advent_volk STATIC volk/volk.c)
add_library(advent::volk ALIAS advent_volk)
target_include_directories(advent_volk SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/volk")
target_compile_definitions(advent_volk PUBLIC VK_NO_PROTOTYPES)
target_link_libraries(advent_volk PUBLIC Vulkan::Headers ${CMAKE_DL_LIBS})

# --- Vulkan Memory Allocator -----------------------------------------------------
# Header-only; the implementation is compiled once in engine/renderer/src/vma_impl.cpp.
add_library(advent_vma INTERFACE)
add_library(advent::vma ALIAS advent_vma)
target_include_directories(advent_vma SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/include")
target_compile_definitions(advent_vma INTERFACE
    VMA_STATIC_VULKAN_FUNCTIONS=0
    VMA_DYNAMIC_VULKAN_FUNCTIONS=0
)
target_link_libraries(advent_vma INTERFACE advent::volk)

# --- stb_image: texture loading ------------------------------------------------
add_library(advent_stb STATIC impl/stb_impl.c)
add_library(advent::stb ALIAS advent_stb)
target_include_directories(advent_stb SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/stb")

# --- cgltf: glTF 2.0 import ------------------------------------------------------
add_library(advent_cgltf STATIC impl/cgltf_impl.c)
add_library(advent::cgltf ALIAS advent_cgltf)
target_include_directories(advent_cgltf SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/cgltf")

# --- nlohmann/json: configs, scenes ---------------------------------------------
add_library(advent_json INTERFACE)
add_library(advent::json ALIAS advent_json)
target_include_directories(advent_json SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/json/single_include")
target_compile_definitions(advent_json INTERFACE JSON_USE_IMPLICIT_CONVERSIONS=0)
