cmake_minimum_required(VERSION 3.25)

project(
    AdventEngine
    VERSION 0.1.0
    DESCRIPTION "Educational 3D game engine on C++20 and Vulkan"
    LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(ADVENT_ENABLE_VALIDATION "Enable Vulkan validation layers and debug names" OFF)
option(ADVENT_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(ADVENT_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(ADVENT_BUILD_SANDBOX "Build the sandbox demo application" ${PROJECT_IS_TOP_LEVEL})

# ---------------------------------------------------------------------------
# Global settings
# ---------------------------------------------------------------------------
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Engine and dependencies are linked statically: no DLLs to copy next to executables.
set(BUILD_SHARED_LIBS OFF)

# Put every executable into one folder so apps find compiled shaders and assets next to them.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AdventWarnings)
include(AdventShaders)

# ---------------------------------------------------------------------------
# Dependencies: vendored copies in vendor/ (see docs/adr/0005)
# ---------------------------------------------------------------------------
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/CMakeLists.txt")
    message(FATAL_ERROR
        "vendor/ is incomplete: vendor/glfw/CMakeLists.txt not found. "
        "Restore it with: git checkout -- vendor")
endif()

add_subdirectory(vendor)

# ---------------------------------------------------------------------------
# Engine, apps, tests
# ---------------------------------------------------------------------------
add_subdirectory(engine)

if(ADVENT_BUILD_SANDBOX)
    add_subdirectory(apps/sandbox)
endif()

if(ADVENT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "AdventEngine ${PROJECT_VERSION}: validation=${ADVENT_ENABLE_VALIDATION}, "
               "tests=${ADVENT_BUILD_TESTS}, sandbox=${ADVENT_BUILD_SANDBOX}")
