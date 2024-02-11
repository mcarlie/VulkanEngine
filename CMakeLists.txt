cmake_minimum_required(VERSION 3.24)

project(VulkanEngine)

set(VulkanEngine_VERSION_MAJOR 0)
set(VulkanEngine_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)

# Include directories to add
include_directories("VulkanEngine/include" "VulkanEngine/src")

# Create VulkanEngine library
file(GLOB SOURCES "VulkanEngine/include/VulkanEngine/*.h" "VulkanEngine/src/*.cpp")
add_library(VulkanEngine ${SOURCES})

# Directory containing libraries
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libs")

## Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
link_directories(AFTER "${VULKAN_SDK}/lib")
target_link_libraries(VulkanEngine ${Vulkan_LIBRARIES})

target_link_libraries(VulkanEngine glslang glslang-default-resource-limits SPIRV)

if( WIN32 )
  add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DNOMINMAX)
endif()

set(ENABLE_VULKAN_VALIDATION OFF CACHE BOOL "Enable Vulkan validation layers")
if(ENABLE_VULKAN_VALIDATION)
  add_definitions(-DENABLE_VULKAN_VALIDATION)
endif()

find_package(glfw3 REQUIRED)
target_link_libraries(VulkanEngine glfw)

# Add header only libraries
include_directories("${LIBS_DIR}/eigen-3.3.5")
include_directories("${LIBS_DIR}/tinyobjloader-1.0.6")
include_directories("${LIBS_DIR}/VulkanMemoryAllocator/include")
include_directories("${LIBS_DIR}/stb")

# Add examples
file(GLOB SOURCES "examples/SimpleScene/*.cpp" "examples/SimpleScene/*.h")
add_executable(SimpleScene ${SOURCES})
target_link_libraries(SimpleScene VulkanEngine)
include_directories("${LIBS_DIR}/cxxopts-2.2.0/include")
if(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY)
  set_property(TARGET SimpleScene APPEND PROPERTY XCODE_SCHEME_ENVIRONMENT ${XCODE_SCHEME_ENVIRONMENT_VARIABLES})
endif()
