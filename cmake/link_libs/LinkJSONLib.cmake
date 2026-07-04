set(JSON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external_libs/json/include")

add_library(json INTERFACE)

target_include_directories(json INTERFACE "${JSON_DIR}")

target_compile_definitions(json INTERFACE
        $<$<CONFIG:Debug>:JSON_DIAGNOSTICS=1>
        $<$<CONFIG:Debug>:JSON_NOEXCEPTION>
        JSON_USE_IMPLICIT_CONVERSIONS=0
)

target_link_libraries(${PROJECT_NAME} PRIVATE json)