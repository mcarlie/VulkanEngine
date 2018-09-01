#ifndef RENDERER_H
#define RENDERER_H

#include <VulkanTest/Window.h>

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

    /// Get the singleton instance of the renderer
    /// Calls the constructor when first called
    static Renderer& getInstance();

    /// Initialize the renderer
    void initialize( const std::shared_ptr< Window >& _window );

  private:

    void createGraphicsPipeline();

    std::shared_ptr< Window > window;

    /// The main Vulkan instance
    vk::Instance vk_instance;

    /// The Vulkan surface object used with the windowing system
    vk::SurfaceKHR vk_surface;

    vk::SwapchainKHR vk_swapchain;

    /// The physical device used for rendering
    vk::PhysicalDevice vk_physical_device;

    /// The logical device used to interface with the physice_device
    vk::Device vk_device;

    vk::Queue vk_graphics_queue;

    std::vector< vk::Image > vk_swapchain_images;

    std::vector< vk::ImageView > vk_image_views;

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