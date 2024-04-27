#ifndef VULKANMANAGER_H
#define VULKANMANAGER_H

#include <VulkanEngine/Camera.h>
#include <VulkanEngine/IndexAttribute.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/Shader.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/VertexAttribute.h>
#include <VulkanEngine/Window.h>

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

  void createCommandBuffers();

  void beginRenderPass();

  void endRenderPass();

  vk::CommandBuffer getCurrentCommandBuffer();

  /// Executes all command buffers and swaps buffers.
  void drawImage();

  /// \return The manager's vk::Device instance.
  const vk::Device &getVkDevice() const;

  /// \return The manager's vk::PhysicalDevice instance.
  const vk::PhysicalDevice &getVKPhysicalDevice() const;

  /// \return The manager's vk::CommandPool for allocating command buffers.
  const vk::CommandPool &getVkCommandPool() const;

  /// \return The manager's vk::Queue for submitting graphics related command
  /// buffers.
  const vk::Queue &getVkGraphicsQueue() const;

  /// \return The VmaAllocator instance for performing allocations with Vulkan
  /// memory allocator.
  const VmaAllocator &getVmaAllocator() const;

  const std::shared_ptr<RenderPass> getDefaultRenderPass() const {
    return default_render_pass;
  }

  const size_t getCurrentFrame() const { return current_frame; }

  const size_t getFramesInFlight() const { return frames_in_flight; }

private:
  void createCommandPool();

  void createSwapchain();
  void createImageViews();

  void createSwapchainFramebuffers();
  void createSyncObjects();

  void cleanup();
  void cleanupSwapchain();

  /// The window instance used with the manager.
  std::shared_ptr<Window> window;

  /// The main Vulkan instance
  vk::Instance vk_instance;

  /// The Vulkan surface object used with the windowing system.
  vk::SurfaceKHR vk_surface;

  vk::SwapchainKHR vk_swapchain;

  /// The physical device used for rendering.
  vk::PhysicalDevice vk_physical_device;

  /// The logical device used to interface with the physice_device.
  vk::Device vk_device;

  VmaAllocator vma_allocator;

  vk::Queue vk_graphics_queue;

  uint32_t graphics_queue_family_index;

  std::vector<vk::Image> vk_swapchain_images;

  std::vector<vk::ImageView> vk_image_views;

  vk::CommandPool vk_command_pool;

  std::vector<vk::CommandBuffer> vk_command_buffers;

  std::vector<vk::Framebuffer> vk_swapchain_framebuffers;

  size_t frames_in_flight;
  size_t current_frame;

  std::vector<vk::Semaphore> vk_image_available_semaphores;
  std::vector<vk::Semaphore> vk_rendering_finished_semaphores;
  std::vector<vk::Fence> vk_in_flight_fences;

  /// Class which allows for dynamic loading of certain functions within Vulkan
  /// classes.
  vk::DispatchLoaderDynamic vk_dispatch_loader_dynamic;

  /// TODO make part of the scene.
  std::shared_ptr<RenderPass> default_render_pass;

  bool initialized;

#ifdef ENABLE_VULKAN_VALIDATION
  vk::DebugUtilsMessengerEXT vk_debug_utils_messenger;

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                VkDebugUtilsMessageTypeFlagsEXT message_type,
                const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                void *user_data);
#endif
};

} // namespace VulkanEngine

#endif /* VULKANMANAGER_H */
