set(GLAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external_libs/glad")

if(BUILD_SHARED_LIBS)
    add_library(glad SHARED "${GLAD_DIR}/src/glad.c")
    target_compile_definitions(glad PRIVATE GLAD_GLAPI_EXPORT)
else()
    add_library(glad STATIC "${GLAD_DIR}/src/glad.c")
endif()

target_include_directories(glad PUBLIC "${GLAD_DIR}/include")

target_link_libraries(${PROJECT_NAME} PRIVATE glad)

if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GLAD_GLAPI_EXPORT)
endif()