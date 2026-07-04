set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external_libs/stb")

if(BUILD_SHARED_LIBS)
    add_library(stb SHARED "${STB_DIR}/stb_image.cpp")
else()
    add_library(stb STATIC "${STB_DIR}/stb_image.cpp")
endif()

target_include_directories(stb PUBLIC "${STB_DIR}")

if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(stb PRIVATE "STBIDEF=__declspec(dllexport)")
    target_compile_definitions(stb INTERFACE "STBIDEF=__declspec(dllimport)")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE stb)