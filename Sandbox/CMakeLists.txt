project(SandboxApp)

add_executable(${PROJECT_NAME} SandboxApp.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC AdventEngine)