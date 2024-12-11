// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_DEVICE_H_
#define INCLUDE_VULKANENGINE_DEVICE_H_

#include <vector>
#include <vulkan/vulkan.hpp>

// Support latest vk_mem_alloc with older Vulkan SDK headers.
#ifndef VK_API_VERSION_MAJOR
#define VK_API_VERSION_MAJOR(version) VK_VERSION_MAJOR(version)
#endif

#ifndef VK_API_VERSION_MINOR
#define VK_API_VERSION_MINOR(version) VK_VERSION_MINOR(version)
#endif

#ifndef VK_API_VERSION_PATCH
#define VK_API_VERSION_PATCH(version) VK_VERSION_PATCH(version)
#endif
#include <vk_mem_alloc.h>

namespace VulkanEngine {

/// Class which represents a Device with vulkan support.
/// TODO this class is in development and unfinished.
class Device {
 public:
  /// Constructor.
  Device();

  /// Destructor.
  ~Device();

  const vk::Device& getVkDevice() const;

  void destroyCommandBuffers();

  void waitIdle();

  vk::CommandBuffer getCommandBuffer(size_t index);

  const VmaAllocator& getVmaAllocator();

  vk::CommandPool getVkCommandPool();

  vk::Queue getVkGraphicsQueue();

  void beginSingleUsageCommandBuffer();

  void endSingleUsageCommandBuffer();

 private:
  int graphics_queue_family_index;
  vk::Device vk_device;
  VmaAllocator vma_allocator;
  vk::Queue vk_graphics_queue;
  vk::CommandPool vk_command_pool;
  std::vector<vk::CommandBuffer> vk_command_buffers;
  vk::CommandBuffer single_use_command_buffer;

  /// Class which allows for dynamic loading of certain functions within Vulkan
  /// classes.
#ifdef __APPLE__
  vk::detail::DispatchLoaderDynamic vk_dispatch_loader_dynamic;
#else
  vk::DispatchLoaderDynamic vk_dispatch_loader_dynamic;
#endif

#ifdef ENABLE_VULKAN_VALIDATION
  vk::DebugUtilsMessengerEXT vk_debug_utils_messenger;

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                VkDebugUtilsMessageTypeFlagsEXT message_type,
                const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                void* user_data);
#endif
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_DEVICE_H_
