set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable GLFW examples" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "Disable GLFW tests" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "Disable GLFW docs" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "Disable GLFW install rules" FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external_libs/glfw")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_DLL)
endif()