cmake_minimum_required(VERSION 3.15...4.1)

project(AdventEngine
        VERSION 0.0.0.1
        DESCRIPTION "The simple game engine for create games"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build vendored dependencies as shared libraries" FORCE)

add_library(${PROJECT_NAME} SHARED src/main.cpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/link_libs/")

include(LinkGLFWLib)
include(LinkGLADLib)
include(LinkGLMLib)
include(LinkJSONLib)
include(LinkSTBLib)