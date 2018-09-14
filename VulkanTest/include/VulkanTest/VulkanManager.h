#ifndef VULKANMANAGER_H
#define VULKANMANAGER_H

#include <VulkanTest/Window.h>
#include <VulkanTest/Shader.h>
#include <VulkanTest/VertexAttribute.h>
#include <VulkanTest/IndexAttribute.h>
#include <VulkanTest/UniformBuffer.h>
#include <VulkanTest/Camera.h>

#include <Eigen/Eigen>

#include <iostream>
#include <memory>

namespace VulkanTest {
  
  class VulkanManager {
  
  private:

    /// Constructor
    /// Private so that an instance may only be created from getInstance()
    VulkanManager();

  public:

    /// Destructor
    ~VulkanManager();

    /// Delete copy constructor to disallow duplicates
    VulkanManager( VulkanManager const& ) = delete;

    /// Delete assignment operator to disallow duplicates
    void operator=( VulkanManager const& ) = delete;

    /// Get the singleton instance of the VulkanManager
    /// Creates the instance when first called
    /// \return The singleton VulkanManager instance
    static std::shared_ptr< VulkanManager >& getInstance();

    /// Initialize the VulkanManager
    /// \param _window The Window instance to use with the manager
    void initialize( const std::shared_ptr< Window >& _window );

    /// Gives the index of the memory type in vk::PhysicalDeviceMemoryProperties::memoryTypes
    /// Which fulfills the requirements given by the flags
    /// \param type_filter vk::MemoryRequirement::memoryTypeBits retrieved for the buffer which will use the memory
    /// \param flags vk::MemoryPropertyFlags indicating desired properties of the buffer 
    uint32_t findMemoryTypeIndex( uint32_t type_filter, vk::MemoryPropertyFlags flags );

    /// Executes all command buffers and swaps buffers
    void drawImage();

    /// \return The manager's vk::Device instance
    vk::Device& getVkDevice();

  private:

    void createSwapchain();
    void createImageViews();
    void createGraphicsPipeline();
    void createRenderPass();
    void createSwapchainFramebuffers();
    void createCommandBuffers();

    void cleanup();
    void cleanupSwapchain();

    /// The window instance used with the manager
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

    vk::DescriptorSetLayout vk_descriptor_set_layout;

    std::vector< vk::Framebuffer > vk_swapchain_framebuffers;

    vk::DescriptorPool vk_descriptor_pool;
    std::vector< vk::DescriptorSet > vk_descriptor_sets;

    struct UniformBufferObject {
      Eigen::Matrix< float, 4, 4 > view;
      Eigen::Matrix< float, 4, 4 > projection;
    };

    std::shared_ptr< Shader > shader;
    std::vector< std::shared_ptr< UniformBuffer< UniformBufferObject > > > uniform_buffers;
    std::shared_ptr< VertexAttribute< Eigen::Vector3f > > position;
    std::shared_ptr< IndexAttribute< uint16_t > > index;

    std::shared_ptr< Camera< float > > camera;

    size_t frames_in_flight;
    size_t current_frame;

    std::vector< vk::Semaphore > vk_image_available_semaphores;
    std::vector< vk::Semaphore > vk_rendering_finished_semaphores;
    std::vector< vk::Fence > vk_in_flight_fences;

    /// Class which allows for dynamic loading of certain functions within Vulkan classes
    vk::DispatchLoaderDynamic vk_dispatch_loader_dynamic;

#if defined( _DEBUG )
    vk::DebugUtilsMessengerEXT vk_debug_utils_messenger;
#endif

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
      VkDebugUtilsMessageTypeFlagsEXT message_type,
      const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
      void* user_data );

  };

}

#endif /* VULKANMANAGER_H */