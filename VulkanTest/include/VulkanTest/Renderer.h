#ifndef RENDERER_H
#define RENDERER_H

#include <VulkanTest/Window.h>

#include <VulkanTest/Shader.h>

#include <iostream>
#include <memory>

namespace VulkanTest {
  
  class Renderer {
  
  private:

    /// Constructor
    /// Private so that an instance may only be created from getInstance()
    Renderer();

  public:

    /// Destructor
    ~Renderer();

    /// Delete copy constructor to disallow duplicates
    Renderer( Renderer const& ) = delete;

    /// Delete assignment operator to disallow duplicates
    void operator=( Renderer const& ) = delete;

    /// Get the singleton instance of the renderer
    /// Creates the instance when first called
    /// \return The singleton renderer instance
    static std::shared_ptr< Renderer >& get();

    /// Initialize the renderer
    /// \param window The Window instance to use with the renderer
    void initialize( const std::shared_ptr< Window >& _window );

    void drawImage();

    vk::Device& getDevice();

  private:

    void createSwapchain();
    void createImageViews();
    void createGraphicsPipeline();
    void createRenderPass();
    void createSwapchainFramebuffers();
    void createCommandBuffers();

    void cleanup();
    void cleanupSwapchain();

    /// The current width of the window's framebuffer
    uint32_t window_fb_width;

    /// The current height of the window's framebuffer
    uint32_t window_fb_height;

    /// The window instance used with the renderer
    std::shared_ptr< Window > window;

    /// The main Vulkan instance
    vk::Instance vk_instance;

    /// The Vulkan surface object used with the windowing system
    vk::SurfaceKHR vk_surface;

    vk::RenderPass vk_render_pass;

    vk::SwapchainKHR vk_swapchain;

    /// The physical device used for rendering
    vk::PhysicalDevice vk_physical_device;

    /// The logical device used to interface with the physice_device
    vk::Device vk_device;

    vk::Queue vk_graphics_queue;

    uint32_t graphics_queue_family_index;

    std::vector< vk::Image > vk_swapchain_images;

    std::vector< vk::ImageView > vk_image_views;

    vk::PipelineLayout vk_layout;

    vk::CommandPool vk_command_pool;

    std::vector< vk::CommandBuffer > vk_command_buffers;

    vk::Pipeline vk_graphics_pipeline;

    std::vector< vk::Framebuffer > vk_swapchain_framebuffers;

    std::shared_ptr< Shader > shader;

    vk::Semaphore vk_image_available_semaphore;
    vk::Semaphore vk_rendering_finished_semaphore;

    /// Class which allows for dynamic loading of certain functions within Vulkan classes
    vk::DispatchLoaderDynamic vk_dispatch_loader_dynamic;

#if defined( _DEBUG )
    vk::DebugUtilsMessengerEXT vk_debug_utils_messenger;
#endif

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData ) {
      std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
      return VK_FALSE;
    }

  };

}

#endif /* RENDERER_H */