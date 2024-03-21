cmake_minimum_required(VERSION 3.22)

project(VulkanEngine)

set(VulkanEngine_VERSION_MAJOR 0)
set(VulkanEngine_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# Include directories to add
include_directories("VulkanEngine/include" "VulkanEngine/src")

# Create VulkanEngine library
file(GLOB VULKAN_ENGINE_SOURCES "VulkanEngine/include/VulkanEngine/*.h" "VulkanEngine/src/*.cpp")
add_library(VulkanEngine ${VULKAN_ENGINE_SOURCES})

# Directory containing libraries
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libs")

## Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
link_directories(AFTER "${VULKAN_SDK}/lib")
target_link_libraries(VulkanEngine ${Vulkan_LIBRARIES})

# On Apple glslang includes default resource limits.
if(NOT APPLE)
  FetchContent_Declare(
    glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
    GIT_TAG        14.1.0
  )

  FetchContent_GetProperties(glslang)
  if(NOT glslang_POPULATED)
    FetchContent_Populate(glslang)
    add_subdirectory(${glslang_SOURCE_DIR} ${glslang_BINARY_DIR})
  endif()

  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH="${glslang_SOURCE_DIR}" python3 "${glslang_SOURCE_DIR}/update_glslang_sources.py"
    WORKING_DIRECTORY ${glslang_SOURCE_DIR}
    RESULT_VARIABLE UPDATE_GLSLANG_RESULT
  )

  if(NOT UPDATE_GLSLANG_RESULT EQUAL "0")
    message(FATAL_ERROR "Failed to update glslang sources")
  endif()

  target_include_directories(VulkanEngine PRIVATE ${glslang_SOURCE_DIR})
endif()

target_link_libraries(VulkanEngine glslang glslang-default-resource-limits SPIRV)

set(ENABLE_VULKAN_VALIDATION OFF CACHE BOOL "Enable Vulkan validation layers")
if(ENABLE_VULKAN_VALIDATION)
  add_definitions(-DENABLE_VULKAN_VALIDATION)
endif()

find_package(glfw3 REQUIRED)
target_link_libraries(VulkanEngine glfw)

find_package (Eigen3 3.4 REQUIRED NO_MODULE)
target_link_libraries (VulkanEngine Eigen3::Eigen)

# Add header only libraries
include_directories("${LIBS_DIR}/tinyobjloader-1.0.6")
include_directories("${LIBS_DIR}/VulkanMemoryAllocator/include")
include_directories("${LIBS_DIR}/stb")

# Add examples
file(GLOB EXAMPLES_SOURCES "examples/SimpleScene/*.cpp" "examples/SimpleScene/*.h")
add_executable(SimpleScene ${EXAMPLES_SOURCES})
target_link_libraries(SimpleScene VulkanEngine)
include_directories("${LIBS_DIR}/cxxopts-2.2.0/include")
if(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY)
  set_property(TARGET SimpleScene APPEND PROPERTY XCODE_SCHEME_ENVIRONMENT ${XCODE_SCHEME_ENVIRONMENT_VARIABLES})
endif()

# Custom target to format all C++ source and header files
if(APPLE)
  add_custom_target(format 
    COMMAND clang-format -i -style=file ${VULKAN_ENGINE_SOURCES} ${EXAMPLES_SOURCES})

  add_dependencies(VulkanEngine format)
endif()

if( WIN32 )
  add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DNOMINMAX)
endif()