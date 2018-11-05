cmake_minimum_required(VERSION 3.7)
project(VulkanEngine)

set(VulkanEngine_VERSION_MAJOR 0)
set(VulkanEngine_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 14)

# Required libraries to link against
set(REQUIRED_LIBS "")

# Include directories to add
include_directories("VulkanEngine/include" "VulkanEngine/src")

# Create VulkanEngine library
file(GLOB SOURCES "VulkanEngine/include/VulkanEngine/*.h" "VulkanEngine/src/*.cpp")
add_library(VulkanEngine ${SOURCES})

# Directory containing libraries
set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libs")

# Add header only libraries
include_directories("${LIBS_DIR}/eigen-3.3.5")
include_directories("${LIBS_DIR}/tinyobjloader-1.0.6")
include_directories("${LIBS_DIR}/VulkanMemoryAllocator-2.1.0/src")

# Makes VULKAN_SDK env variable settable by user
if(DEFINED ENV{VULKAN_SDK})
  set(DEFAULT_VULKAN_SDK_PATH $ENV{VULKAN_SDK})
else()
  set(DEFAULT_VULKAN_SDK_PATH "")
endif()
set(VULKAN_SDK ${DEFAULT_VULKAN_SDK_PATH} CACHE PATH "Path to Vulkan SDK")
set(ENV{VULKAN_SDK} ${VULKAN_SDK})

# Makes VK_ICD_FILENAMES env variable settable by user
if(APPLE)
  if(DEFINED ENV{VK_ICD_FILENAMES})
    set( DEFAULT_VK_ICD_FILENAMES $ENV{VK_ICD_FILENAMES} )
  elseif(VULKAN_SDK STREQUAL "")
    set(DEFAULT_VK_ICD_FILENAMES "")
  else()
    set(DEFAULT_VK_ICD_FILENAMES "${VULKAN_SDK}/etc/vulkan/icd.d/MoltenVK_icd.json")
  endif()
endif()
set( VK_ICD_FILENAMES ${DEFAULT_VK_ICD_FILENAMES} CACHE PATH "Path to the MoltenVK ICD" )
set( ENV{VK_ICD_FILENAMES} ${VULKAN_SDK} )

# Vulkan
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

# Add examples
include_directories("${LIBS_DIR}/stb")

file( GLOB SOURCES "examples/SimpleScene/*.cpp" "examples/SimpleScene/*.h")
add_executable(SimpleScene ${SOURCES})
target_link_libraries(SimpleScene VulkanEngine)
