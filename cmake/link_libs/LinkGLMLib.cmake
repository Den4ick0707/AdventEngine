set(GLM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external_libs/glm")

add_library(glm INTERFACE)

target_include_directories(glm INTERFACE "${GLM_DIR}")

target_compile_definitions(glm INTERFACE
        GLM_FORCE_RADIANS
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_FORCE_INLINE
        GLM_FORCE_AVX
        GLM_FORCE_AVX2
        GLM_FORCE_SSE42
)

target_link_libraries(${PROJECT_NAME} PRIVATE glm)