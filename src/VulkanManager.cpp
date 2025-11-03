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

#include <VulkanEngine/Framebuffer.h>
#include <VulkanEngine/Image.h>
#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/Swapchain.h>
#include <VulkanEngine/VulkanManager.h>

#include <iostream>
#include <memory>
#include <vector>

#include "vulkan/vulkan_core.h"

#define VMA_IMPLEMENTATION
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#endif
#include <vk_mem_alloc.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

VulkanEngine::VulkanManager::VulkanManager()
    : frames_in_flight(3),
      current_frame(0),
      window(nullptr),
      initialized(false) {}

VulkanEngine::VulkanManager::~VulkanManager() { cleanup(); }

std::shared_ptr<VulkanEngine::VulkanManager>&
VulkanEngine::VulkanManager::getInstanceInternal() {
  static std::shared_ptr<VulkanManager> instance;
  if (instance.get() == nullptr) {
    instance.reset(new VulkanManager());
  }
  return instance;
}

VulkanEngine::VulkanManager& VulkanEngine::VulkanManager::getInstance() {
  return *(getInstanceInternal().get());
}

void VulkanEngine::VulkanManager::resetInstance() {
  getInstance().cleanup();
  getInstanceInternal().reset();
}

bool VulkanEngine::VulkanManager::initialize(
    const std::shared_ptr<Window> _window) {
  initialized = true;

  window = _window;

  try {
    std::vector<const char*> instance_extensions(
        window->getRequiredVulkanInstanceExtensions());

    // Use validation layers if this is a debug build
    std::vector<const char*> layers;

#ifdef ENABLE_VULKAN_VALIDATION
    layers.push_back("VK_LAYER_KHRONOS_validation");
    instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    instance_extensions.push_back("VK_KHR_get_physical_device_properties2");
    instance_extensions.push_back("VK_KHR_device_group_creation");
    instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef __APPLE__
    instance_extensions.push_back(
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
    instance_extensions.push_back(
        VK_KHR_EXTERNAL_FENCE_CAPABILITIES_EXTENSION_NAME);
    instance_extensions.push_back(
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
    instance_extensions.push_back(
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME);

    auto app_info = vk::ApplicationInfo()
                        .setPApplicationName("VulkanEngine")
                        .setApplicationVersion(1)
                        .setPEngineName("No engine")
                        .setEngineVersion(1)
                        .setApiVersion(VK_API_VERSION_1_1);

    auto inst_info =
        vk::InstanceCreateInfo()
#ifdef __APPLE__
            .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
#endif
            .setPApplicationInfo(&app_info)
            .setEnabledExtensionCount(
                static_cast<uint32_t>(instance_extensions.size()))
            .setPpEnabledExtensionNames(instance_extensions.data())
            .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
            .setPpEnabledLayerNames(layers.data());

    vk_instance = vk::createInstance(inst_info);

    device.reset(new Device());

    default_render_pass.reset(new RenderPass(window->getFramebufferWidth(),
                                             window->getFramebufferHeight()));
    swapchain.reset(
        new Swapchain(frames_in_flight, window, default_render_pass));
  } catch (const std::exception& e) {
    std::cerr << "Exception during engine initialization: " << e.what()
              << std::endl;
    return false;
  } catch (...) {
    std::cerr << "An unknown error occurred during engine initialization."
              << std::endl;
    return false;
  }

  return true;
}

vk::CommandBuffer VulkanEngine::VulkanManager::getCurrentCommandBuffer() {
  return device->getCommandBuffer(current_frame);
}

vk::Framebuffer VulkanEngine::VulkanManager::getCurrentSwapchainFramebuffer() {
  return swapchain->getFramebuffer(current_frame);
}

void VulkanEngine::VulkanManager::waitForFence() { swapchain->waitForFence(); }

void VulkanEngine::VulkanManager::drawImage() {
  // Submit commands to the queue
  if (!swapchain->present()) {
    device->waitIdle();
    default_render_pass.reset();
    default_render_pass.reset(new RenderPass(window->getFramebufferWidth(),
                                             window->getFramebufferHeight()));
    swapchain.reset();
    swapchain.reset(
        new Swapchain(frames_in_flight, window, default_render_pass));
    current_frame = 0;
    return;
  }

  current_frame = (current_frame + 1) % frames_in_flight;
}

void VulkanEngine::VulkanManager::cleanup() {
  if (!initialized) {
    return;
  }

  device->waitIdle();
  default_render_pass.reset();
  swapchain.reset();

  device.reset();
  window.reset();
  vk_instance.destroy();

  initialized = false;
}
