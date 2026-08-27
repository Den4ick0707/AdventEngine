find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
    message(STATUS "Vulkan found in ${Vulkan_INCLUDE_DIR}")
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
endif()