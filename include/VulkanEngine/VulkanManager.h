#ifndef VULKANMANAGER_H
#define VULKANMANAGER_H

#include <VulkanEngine/Camera.h>
#include <VulkanEngine/IndexAttribute.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/Shader.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/VertexAttribute.h>
#include <VulkanEngine/Window.h>
#include <VulkanEngine/Device.h>

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

#include <Eigen/Eigen>
#include <iostream>
#include <memory>

namespace VulkanEngine {

// Forward decleration
template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
class Image;
class RenderPass;
class Framebuffer;
class Swapchain;

/// TODO this class is a work in progress. The current goal is to modulerize
/// this more and place functionality in seperate classes.
class VulkanManager {
private:
  /// Constructor.
  /// Private so that an instance may only be created from getInstance().
  VulkanManager();

  /// Used for internal singleton handling.
  static std::shared_ptr<VulkanManager>& getInstanceInternal();

public:
  /// Destructor.
  ~VulkanManager();

  /// Delete copy constructor to disallow duplicates.
  VulkanManager(const VulkanManager &) = delete;

  /// Delete assignment operator to disallow duplicates.
  void operator=(const VulkanManager &) = delete;

  /// Get the singleton instance of the VulkanManager.
  /// Creates the instance when first called.
  /// \return The singleton VulkanManager instance.
  static VulkanManager& getInstance();

  /// Reset the singleton instance.
  static void resetInstance();

  /// Destroy the singleton instance of the VulkanManager.
  static void destroyInstance();

  /// Initialize the VulkanManager.
  /// \param _window The Window instance to use with the manager.
  bool initialize(const std::shared_ptr<Window> _window);

  vk::CommandBuffer getCurrentCommandBuffer();

  void waitForFence();

  vk::Framebuffer getCurrentSwapchainFramebuffer();

  /// Executes all command buffers and swaps buffers.
  void drawImage();

  std::shared_ptr<Device> getDevice() {
    return device;
  }

  vk::Instance getVkInstance() const {
    return vk_instance;
  }

  const std::shared_ptr<RenderPass> getDefaultRenderPass() const {
    return default_render_pass;
  }

  const size_t getCurrentFrame() const { return current_frame; }

  const size_t getFramesInFlight() const { return frames_in_flight; }

private:

  void cleanup();

  /// The main Vulkan instance
  vk::Instance vk_instance;

  /// The window instance used with the manager.
  std::shared_ptr<Window> window;

  std::shared_ptr<Device> device;
  std::shared_ptr<Swapchain> swapchain;
  std::shared_ptr<RenderPass> default_render_pass;

  size_t frames_in_flight;
  size_t current_frame;

  bool initialized;
};

} // namespace VulkanEngine

#endif /* VULKANMANAGER_H */
