cmake_minimum_required(VERSION 3.16)

project(VulkanEngine)

set(VulkanEngine_VERSION_MAJOR 0)
set(VulkanEngine_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build the engine tests." OFF)

include(FetchContent)

# Include directories to add
include_directories("src/include" "src")

set(VULKAN_ENGINE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Create VulkanEngine library
file(GLOB VULKAN_ENGINE_SOURCES "src/*.cpp")
file(GLOB VULKAN_ENGINE_HEADERS "include/VulkanEngine/*.h")

include_directories(${VULKAN_ENGINE_INCLUDE_DIR})
add_library(VulkanEngine STATIC ${VULKAN_ENGINE_SOURCES} ${VULKAN_ENGINE_HEADERS})

# Look for clang-format
find_program(CLANG_FORMAT "clang-format")
if(APPLE AND CLANG_FORMAT)
  add_custom_target(format 
    COMMAND ${CLANG_FORMAT} -i -style=file ${VULKAN_ENGINE_SOURCES} ${EXAMPLES_SOURCES})

  add_dependencies(VulkanEngine format)
else()
  message(STATUS "clang-format not found, skipping format target")
endif()

target_include_directories(VulkanEngine PUBLIC ${VULKAN_ENGINE_INCLUDE_DIR})

# Directory containing libraries
set(THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

## Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
link_directories(AFTER "${VULKAN_SDK}/lib")
target_link_libraries(VulkanEngine ${Vulkan_LIBRARIES})

find_package(Threads)

# glslang
include(FetchContent)
# Try to find glslang locally first
find_package(glslang QUIET)
if(NOT glslang_FOUND)  # If not found, fetch and build it
    message(STATUS "glslang not found locally. Fetching from repository...")
    FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG        12.3.1
    )
    FetchContent_MakeAvailable(glslang)
    set(glslang_lib_name_prefix "")
else()
    message(STATUS "Found glslang locally: ${glslang_VERSION}")
    set(glslang_lib_name_prefix "glslang::")
endif()

target_link_libraries(VulkanEngine
  ${glslang_lib_name_prefix}glslang 
  ${glslang_lib_name_prefix}glslang-default-resource-limits 
  ${glslang_lib_name_prefix}SPIRV 
)

set(ENABLE_VULKAN_VALIDATION OFF CACHE BOOL "Enable Vulkan validation layers")
if(ENABLE_VULKAN_VALIDATION)
  add_definitions(-DENABLE_VULKAN_VALIDATION)
endif()

find_package(glfw3 REQUIRED)
target_link_libraries(VulkanEngine glfw)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)
target_link_libraries(VulkanEngine Eigen3::Eigen)

# Add header only libraries
include_directories("${THIRDPARTY_DIR}/tinyobjloader-1.0.6")
include_directories("${THIRDPARTY_DIR}/VulkanMemoryAllocator")
include_directories("${THIRDPARTY_DIR}/stb")

# Add examples
add_subdirectory(examples)

# Add tests if enabled
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if( WIN32 )
  add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DNOMINMAX)
endif()