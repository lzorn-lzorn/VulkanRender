# CMake minimum version requirement
cmake_minimum_required(VERSION 3.16)

# Project name and version
project(Render VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_C_FLAGS "-m64")
# set(CMAKE_CXX_FLAGS "-m64")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Add subdirectories first
add_subdirectory(SDL EXCLUDE_FROM_ALL)
add_subdirectory(VulkanRHI)

# Find Vulkan (needed by main project as well)
find_package(Vulkan REQUIRED)

    # Create executable with all source files
add_executable(${PROJECT_NAME} main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PUBLIC VulkanRHI)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})

# Set target properties
target_include_directories(${PROJECT_NAME} PRIVATE 
    VulkanRHI/Include
    ${Vulkan_INCLUDE_DIRS}
)

# Workaround for Vulkan SDK 1.4 + Clang compatibility issue
# if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#     target_compile_definitions(${PROJECT_NAME} PRIVATE 
#         VULKAN_HPP_NO_STRUCT_COMPARE 
#         VULKAN_HPP_NO_SPACESHIP_OPERATOR
#     )
# endif()

# Print build information
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Vulkan Include Dirs: ${Vulkan_INCLUDE_DIRS}")