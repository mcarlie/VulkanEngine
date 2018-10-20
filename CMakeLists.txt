cmake_minimum_required(VERSION 3.7)
project(VulkanEngine)

set(VulkanEngine_VERSION_MAJOR 0)
set(VulkanEngine_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 11)

# Required libraries to link against
set(REQUIRED_LIBS "")

# Include directories to add
include_directories("VulkanEngine/include" "VulkanEngine/src")

# Directory containing libraries
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libs")

# Add header only libraries
include_directories("${LIBS_DIR}/eigen-3.3.5" )
include_directories("${LIBS_DIR}/tinyobjloader-1.0.6" )
include_directories("${LIBS_DIR}/stb" )
include_directories("${LIBS_DIR}/VulkanMemoryAllocator-2.1.0/src" )

# Add sources to project
file( GLOB SOURCES "VulkanEngine/include/VulkanEngine/*.h" "VulkanEngine/src/*.cpp" )
add_executable(VulkanEngine ${SOURCES})

# Find Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
target_link_libraries(VulkanEngine ${Vulkan_LIBRARIES})

# GLFW3
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${LIBS_DIR}/glfw-3.2.1")
target_link_libraries(VulkanEngine glfw)