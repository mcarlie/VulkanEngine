// Copyright (c) 2025 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <VulkanEngine/Device.h>
#include <VulkanEngine/VulkanManager.h>

#include <iostream>
#include <limits>
#include <vector>

VulkanEngine::Device::Device() : graphics_queue_family_index(0) {
  auto& vulkan_manager = VulkanManager::getInstance();
  auto vk_instance = vulkan_manager.getVkInstance();

#ifdef ENABLE_VULKAN_VALIDATION
  vk::DebugUtilsMessengerCreateInfoEXT debug_message_create_info;
  debug_message_create_info.messageSeverity =
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
  debug_message_create_info.messageType =
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
  debug_message_create_info.pfnUserCallback = debugCallback;
  debug_message_create_info.pUserData = nullptr;

  vk_dispatch_loader_dynamic.init(vk_instance, vkGetInstanceProcAddr, vk_device,
                                  vkGetDeviceProcAddr);
  vk_debug_utils_messenger = vk_instance.createDebugUtilsMessengerEXT(
      debug_message_create_info, nullptr, vk_dispatch_loader_dynamic);

  if (!vk_debug_utils_messenger) {
    throw std::runtime_error(
        "Could not initialize debug reporting for vulkan!");
  }
#endif

  // TODO(michael) Check suitability and handle case where there is no device
  std::vector<vk::PhysicalDevice> physical_devices =
      vk_instance.enumeratePhysicalDevices();

  if (physical_devices.empty()) {
    throw std::runtime_error(
        "Could not find a valid physical device for rendering.");
  }
  auto vk_physical_device = physical_devices[0];
  vk::PhysicalDeviceProperties device_properties =
      vk_physical_device.getProperties();
  std::cout << "Chosen device: " << device_properties.deviceName << std::endl;

  std::vector<vk::ExtensionProperties> physical_device_extensions =
      vk_physical_device.enumerateDeviceExtensionProperties();

  std::vector<const char*> physical_device_extension_names;
  for (const auto& ext : physical_device_extensions) {
    physical_device_extension_names.push_back(ext.extensionName);
  }

#ifdef __APPLE__
  physical_device_extension_names.push_back(
      VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME);
  physical_device_extension_names.push_back(
      VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
#endif

  // Find queue family with graphics support
  std::vector<vk::QueueFamilyProperties> queue_family_properties =
      vk_physical_device.getQueueFamilyProperties();
  for (auto& q : queue_family_properties) {
    if (q.queueFlags & vk::QueueFlagBits::eGraphics) {
      break;
    }
    ++graphics_queue_family_index;
  }

  float queue_priorities[] = {1.0f};
  auto queue_info = vk::DeviceQueueCreateInfo()
                        .setPQueuePriorities(queue_priorities)
                        .setQueueCount(1)
                        .setQueueFamilyIndex(graphics_queue_family_index);

  auto physical_device_features = vk::PhysicalDeviceFeatures()
                                      .setSamplerAnisotropy(VK_TRUE)
                                      .setFragmentStoresAndAtomics(VK_TRUE)
                                      .setSampleRateShading(VK_TRUE);

  std::vector<const char*> layers;
#ifdef ENABLE_VULKAN_VALIDATION
  layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

  auto device_info =
      vk::DeviceCreateInfo()
          .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
          .setPpEnabledLayerNames(layers.data())
          .setPQueueCreateInfos(&queue_info)
          .setQueueCreateInfoCount(1)
          .setPEnabledFeatures(&physical_device_features)
          .setPpEnabledExtensionNames(physical_device_extension_names.data())
          .setEnabledExtensionCount(
              static_cast<uint32_t>(physical_device_extension_names.size()));

  vk_device = vk_physical_device.createDevice(device_info);

  VmaAllocatorCreateInfo vma_allocator_create_info = {};
  vma_allocator_create_info.device = vk_device;
  vma_allocator_create_info.physicalDevice = vk_physical_device;
  vma_allocator_create_info.instance = vk_instance;

  if (vmaCreateAllocator(&vma_allocator_create_info, &vma_allocator) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create VmaAllocator!");
  }

  vk_graphics_queue = vk_device.getQueue(graphics_queue_family_index, 0);

  // TODO(michael): Use these
  // auto surface_formats =
  // vk_physical_device.getSurfaceFormatsKHR(vk_surface); auto present_modes =
  //     vk_physical_device.getSurfacePresentModesKHR(vk_surface);
  // auto surface_support = vk_physical_device.getSurfaceSupportKHR(
  //     graphics_queue_family_index, vk_surface);

  vk::CommandPoolCreateInfo command_pool_info(
      vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      graphics_queue_family_index);

  vk_command_pool = vk_device.createCommandPool(command_pool_info);

  auto command_buffer_allocate_info =
      vk::CommandBufferAllocateInfo()
          .setCommandBufferCount(vulkan_manager.getFramesInFlight())
          .setCommandPool(vk_command_pool)
          .setLevel(vk::CommandBufferLevel::ePrimary);

  vk_command_buffers =
      vk_device.allocateCommandBuffers(command_buffer_allocate_info);
}

VulkanEngine::Device::~Device() {
  destroyCommandBuffers();
  vk_device.destroyCommandPool(vk_command_pool);
  vmaDestroyAllocator(vma_allocator);
  vma_allocator = nullptr;
  vk_device.destroy();
#ifdef ENABLE_VULKAN_VALIDATION
  VulkanManager::getInstance().getVkInstance().destroyDebugUtilsMessengerEXT(
      vk_debug_utils_messenger, nullptr, vk_dispatch_loader_dynamic);
#endif
}

const vk::Device& VulkanEngine::Device::getVkDevice() const {
  return vk_device;
}

void VulkanEngine::Device::destroyCommandBuffers() {
  if (vk_command_buffers.size() != 0) {
    vk_device.freeCommandBuffers(vk_command_pool, vk_command_buffers);
  }
  vk_command_buffers.clear();
}

void VulkanEngine::Device::waitIdle() { vk_device.waitIdle(); }

vk::CommandBuffer VulkanEngine::Device::getCommandBuffer(size_t index) {
  return vk_command_buffers[index];
}

const VmaAllocator& VulkanEngine::Device::getVmaAllocator() {
  return vma_allocator;
}

vk::CommandPool VulkanEngine::Device::getVkCommandPool() {
  return vk_command_pool;
}

vk::Queue VulkanEngine::Device::getVkGraphicsQueue() {
  return vk_graphics_queue;
}

void VulkanEngine::Device::beginSingleUsageCommandBuffer() {
  auto command_buffer_info = vk::CommandBufferAllocateInfo()
                                 .setCommandBufferCount(1)
                                 .setCommandPool(vk_command_pool)
                                 .setLevel(vk::CommandBufferLevel::ePrimary);

  auto command_buffers = vk_device.allocateCommandBuffers(command_buffer_info);
  if (command_buffers.empty()) {
    throw std::runtime_error(
        "Could not allocate command buffer for buffer transfer!");
  }

  single_use_command_buffer = command_buffers[0];

  auto command_buffer_begin_info = vk::CommandBufferBeginInfo().setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  single_use_command_buffer.begin(command_buffer_begin_info);
}

void VulkanEngine::Device::endSingleUsageCommandBuffer() {
  single_use_command_buffer.end();

  vk::FenceCreateInfo fenceInfo;
  vk::Fence fence = vk_device.createFence(fenceInfo);

  auto submit_info =
      vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(
          &single_use_command_buffer);

  vk_graphics_queue.submit(submit_info, fence);
  auto fence_result = vk_device.waitForFences(
      fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
  if (fence_result != vk::Result::eSuccess) {
    throw std::runtime_error("Error waiting for fences");
  }
  vk_device.destroyFence(fence);
  vk_device.freeCommandBuffers(vk_command_pool, single_use_command_buffer);
}

#ifdef ENABLE_VULKAN_VALIDATION
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngine::Device::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
  std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
  return VK_FALSE;
}
#endif
