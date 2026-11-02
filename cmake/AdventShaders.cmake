# advent_compile_shaders(<target> SOURCES <file>...)
#
# Compiles GLSL sources (*.vert, *.frag, *.comp, ...) to SPIR-V with glslc and makes
# <target> depend on the result. Output: ${CMAKE_BINARY_DIR}/bin/shaders/<name>.<stage>.spv
# The shader directory is passed to <target> as the ADVENT_SHADER_DIR compile definition.
#
# glslc comes with the Vulkan SDK (or the `glslc` package on Linux).

function(advent_compile_shaders target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "" "SOURCES")
    if(NOT arg_SOURCES)
        message(FATAL_ERROR "advent_compile_shaders(${target}): SOURCES is empty")
    endif()

    find_program(ADVENT_GLSLC glslc
        HINTS "$ENV{VULKAN_SDK}/bin" "$ENV{VULKAN_SDK}/Bin"
        DOC "Path to the glslc shader compiler")
    if(NOT ADVENT_GLSLC)
        message(FATAL_ERROR
            "glslc not found. Install the Vulkan SDK (https://vulkan.lunarg.com) "
            "and make sure the VULKAN_SDK environment variable is set.")
    endif()

    set(shader_dir "${CMAKE_BINARY_DIR}/bin/shaders")
    set(include_dir "${PROJECT_SOURCE_DIR}/shaders/common")
    set(outputs "")

    foreach(source IN LISTS arg_SOURCES)
        cmake_path(ABSOLUTE_PATH source BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE source_abs)
        cmake_path(GET source_abs FILENAME name)
        set(output "${shader_dir}/${name}.spv")
        set(depfile "${output}.d")

        add_custom_command(
            OUTPUT "${output}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${shader_dir}"
            COMMAND "${ADVENT_GLSLC}" --target-env=vulkan1.3 -I "${include_dir}"
                    $<$<CONFIG:Debug>:-g> -MD -MF "${depfile}"
                    -o "${output}" "${source_abs}"
            MAIN_DEPENDENCY "${source_abs}"
            DEPFILE "${depfile}"
            COMMENT "glslc ${name}"
            VERBATIM
        )
        list(APPEND outputs "${output}")
    endforeach()

    add_custom_target(${target}_shaders DEPENDS ${outputs})
    add_dependencies(${target} ${target}_shaders)
    target_compile_definitions(${target} PRIVATE ADVENT_SHADER_DIR="${shader_dir}")
endfunction()
