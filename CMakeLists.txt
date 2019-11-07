cmake_minimum_required(VERSION 3.7)

set(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY FALSE)
if((CMAKE_GENERATOR STREQUAL Xcode) AND NOT (${CMAKE_VERSION} VERSION_LESS "3.13.5"))
  set(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY TRUE)
endif()

if((SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY))
  set(ADD_VULKAN_ENVIRONMENT_VARIABLES_TO_XCODE_SCHEME OFF CACHE BOOL "Add VK_ICD_FILENAMES and VK_LAYER_PATH to Xcode scheme")
endif()

if(NOT ADD_VULKAN_ENVIRONMENT_VARIABLES_TO_XCODE_SCHEME)
  set(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY FALSE)
endif()

if(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY)
  set(CMAKE_XCODE_GENERATE_SCHEME ON)
  set(XCODE_SCHEME_ENVIRONMENT ON)
endif()

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

# Makes VULKAN_SDK env variable settable by user
if(DEFINED ENV{VULKAN_SDK})
  set(DEFAULT_VULKAN_SDK_PATH $ENV{VULKAN_SDK})
else()
  set(DEFAULT_VULKAN_SDK_PATH "")
endif()
set(VULKAN_SDK ${DEFAULT_VULKAN_SDK_PATH} CACHE PATH "Path to Vulkan SDK")
set(ENV{VULKAN_SDK} ${VULKAN_SDK})

## Vulkan
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
link_directories(AFTER "${VULKAN_SDK}/lib")
target_link_libraries(VulkanEngine ${Vulkan_LIBRARIES})

add_subdirectory(${LIBS_DIR}/glslang)
include_directories(${LIBS_DIR}/glslang)
target_link_libraries(VulkanEngine glslang SPIRV )

# Makes VK_ICD_FILENAMES env variable settable by user and add to Xcode scheme
if(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY)
  set(DEFAULT_VK_ICD_FILENAMES "")
  if(DEFINED ENV{VK_ICD_FILENAMES})
    set( DEFAULT_VK_ICD_FILENAMES $ENV{VK_ICD_FILENAMES} )
  elseif(NOT VULKAN_SDK STREQUAL "")
    set(DEFAULT_VK_ICD_FILENAMES ${VULKAN_SDK}/etc/vulkan/icd.d/MoltenVK_icd.json)
  endif()
  set(VK_ICD_FILENAMES ${DEFAULT_VK_ICD_FILENAMES} CACHE PATH "Path to the MoltenVK ICD")
  list(APPEND XCODE_SCHEME_ENVIRONMENT_VARIABLES "VK_ICD_FILENAMES=${VK_ICD_FILENAMES}")
endif()

# Makes VK_LAYER_PATH env variable settable by user and add to Xcode scheme
if(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY)
  set(DEFAULT_VK_ICD_FILENAMES "")
  if(DEFINED ENV{VK_LAYER_PATH})
    set( DEFAULT_VK_LAYER_PATH $ENV{VK_LAYER_PATH} )
  elseif(NOT VULKAN_SDK STREQUAL "")
    set(DEFAULT_VK_LAYER_PATH "${VULKAN_SDK}/etc/vulkan/explicit_layer.d")
  endif()
  set(VK_LAYER_PATH ${DEFAULT_VK_LAYER_PATH} CACHE PATH "Path to the Vulkan layer JSON files")
  list(APPEND XCODE_SCHEME_ENVIRONMENT_VARIABLES "VK_LAYER_PATH=${VK_LAYER_PATH}")
endif()

# GLFW3
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${LIBS_DIR}/glfw")
target_link_libraries(VulkanEngine glfw)

# Add header only libraries
include_directories("${LIBS_DIR}/eigen-3.3.5")
include_directories("${LIBS_DIR}/tinyobjloader-1.0.6")
include_directories("${LIBS_DIR}/VulkanMemoryAllocator-2.1.0/src")
include_directories("${LIBS_DIR}/stb")

# Add examples
file(GLOB SOURCES "examples/SimpleScene/*.cpp" "examples/SimpleScene/*.h")
add_executable(SimpleScene ${SOURCES})
target_link_libraries(SimpleScene VulkanEngine)
include_directories("${LIBS_DIR}/cxxopts-2.2.0/include")
if(SUPPORT_XCODE_SCHEME_ENVIRONMENT_PROPERTY)
  set_property(TARGET SimpleScene APPEND PROPERTY XCODE_SCHEME_ENVIRONMENT ${XCODE_SCHEME_ENVIRONMENT_VARIABLES})
endif()
