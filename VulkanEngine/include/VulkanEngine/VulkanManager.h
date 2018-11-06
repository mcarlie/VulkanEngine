#ifndef VULKANMANAGER_H
#define VULKANMANAGER_H

#include <VulkanEngine/Window.h>
#include <VulkanEngine/Shader.h>
#include <VulkanEngine/VertexAttribute.h>
#include <VulkanEngine/IndexAttribute.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/Camera.h>
#include <VulkanEngine/MeshBase.h>

#include <Eigen/Eigen>
#include <vk_mem_alloc.h>

#include <iostream>
#include <memory>

namespace VulkanEngine {

  // Forward decleration
  template< vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling, vk::SampleCountFlagBits sample_count_flags >
  class Image;
  
  class VulkanManager {
  
  private:

    /// Constructor.
    /// Private so that an instance may only be created from getInstance().
    VulkanManager();

  public:

    /// Destructor.
    ~VulkanManager();

    /// Delete copy constructor to disallow duplicates.
    VulkanManager( const VulkanManager& ) = delete;

    /// Delete assignment operator to disallow duplicates.
    void operator=( const VulkanManager& ) = delete;

    /// Get the singleton instance of the VulkanManager.
    /// Creates the instance when first called.
    /// \return The singleton VulkanManager instance.
    static std::shared_ptr< VulkanManager >& getInstance();

    /// Initialize the VulkanManager.
    /// \param _window The Window instance to use with the manager.
    void initialize( const std::shared_ptr< Window >& _window );

    void createGraphicsPipeline( const std::shared_ptr< MeshBase >& mesh, const std::shared_ptr< Shader >& shader );

    void createCommandBuffers( const std::shared_ptr< MeshBase >& mesh, const std::shared_ptr< Shader > shader );

    /// Executes all command buffers and swaps buffers.
    void drawImage();

    /// \return The manager's vk::Device instance.
    const vk::Device& getVkDevice();

    /// \return The manager's vk::PhysicalDevice instance.
    const vk::PhysicalDevice& getVKPhysicalDevice();

    /// \return The manager's vk::CommandPool for allocating command buffers.
    const vk::CommandPool& getVkCommandPool();

    /// \return The manager's vk::Queue for submitting graphics related command buffers.
    const vk::Queue& getVkGraphicsQueue();

    /// \return The VmaAllocator instance for performing allocations with Vulkan memory allocator.
    const VmaAllocator& getVmaAllocator();

  private:

    void createSwapchain();
    void createImageViews();

    void createRenderPass();
    void createSwapchainFramebuffers();
    void createSyncObjects();

    void cleanup();
    void cleanupSwapchain();

    /// The window instance used with the manager.
    std::shared_ptr< Window > window;

    /// The main Vulkan instance
    vk::Instance vk_instance;

    /// The Vulkan surface object used with the windowing system.
    vk::SurfaceKHR vk_surface;

    vk::RenderPass vk_render_pass;

    vk::SwapchainKHR vk_swapchain;

    /// The physical device used for rendering.
    vk::PhysicalDevice vk_physical_device;

    /// The logical device used to interface with the physice_device.
    vk::Device vk_device;

    VmaAllocator vma_allocator;

    vk::Queue vk_graphics_queue;

    uint32_t graphics_queue_family_index;

    std::vector< vk::Image > vk_swapchain_images;

    std::vector< vk::ImageView > vk_image_views;

    vk::CommandPool vk_command_pool;

    std::vector< vk::CommandBuffer > vk_command_buffers;

    vk::Pipeline vk_graphics_pipeline;

    std::vector< vk::Framebuffer > vk_swapchain_framebuffers;

    using DepthStencilImageAttachment 
      = Image< vk::Format::eD24UnormS8Uint, vk::ImageType::e2D, vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e8 >;

    using ColorAttachment
      = Image< vk::Format::eB8G8R8A8Unorm, vk::ImageType::e2D, vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e8 >;

    std::shared_ptr< DepthStencilImageAttachment > depth_stencil_attachment;
    std::shared_ptr< ColorAttachment > color_attachment;

    size_t frames_in_flight;
    size_t current_frame;

    std::vector< vk::Semaphore > vk_image_available_semaphores;
    std::vector< vk::Semaphore > vk_rendering_finished_semaphores;
    std::vector< vk::Fence > vk_in_flight_fences;

    /// Class which allows for dynamic loading of certain functions within Vulkan classes.
    vk::DispatchLoaderDynamic vk_dispatch_loader_dynamic;

#if defined( _DEBUG )
    vk::DebugUtilsMessengerEXT vk_debug_utils_messenger;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
      VkDebugUtilsMessageTypeFlagsEXT message_type,
      const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
      void* user_data );
#endif

  };

}

#endif /* VULKANMANAGER_H */
