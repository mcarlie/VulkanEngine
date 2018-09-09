#include <VulkanTest/Renderer.h>
#include <VulkanTest/VertexAttribute.h>

VulkanTest::Renderer::Renderer() : window_fb_width( 0 ), window_fb_height( 0 ) {
}

VulkanTest::Renderer::~Renderer() {
  vk_device.waitIdle();
  cleanup();
}

std::shared_ptr< VulkanTest::Renderer >& VulkanTest::Renderer::get() {
  static std::shared_ptr< VulkanTest::Renderer > singleton_renderer_instance( new Renderer() );
  return singleton_renderer_instance;
}

void VulkanTest::Renderer::initialize( const std::shared_ptr< Window >& _window ) {

  window = _window;
  window_fb_width = window->getFramebufferWidth();
  window_fb_height = window->getFramebufferHeight();

  std::vector< const char* > instance_extensions( window->getRequiredVulkanInstanceExtensions() );

  // Use validation layers if this is a debug build
  std::vector< const char* > layers;
#if defined( _DEBUG )
  layers.push_back("VK_LAYER_LUNARG_standard_validation");
  instance_extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
#endif

  auto app_info = vk::ApplicationInfo()
    .setPApplicationName( "VulkanTest" )
    .setApplicationVersion( 1 )
    .setPEngineName( "No engine" )
    .setEngineVersion( 1 )
    .setApiVersion( VK_API_VERSION_1_1 );

  auto inst_info = vk::InstanceCreateInfo()
    .setFlags( vk::InstanceCreateFlags() )
    .setPApplicationInfo( &app_info )
    .setEnabledExtensionCount( static_cast< uint32_t >( instance_extensions.size() ) )
    .setPpEnabledExtensionNames( instance_extensions.data() )
    .setEnabledLayerCount( static_cast< uint32_t >( layers.size() ) )
    .setPpEnabledLayerNames( layers.data() );

  vk_instance = vk::createInstance( inst_info );

  // Setup debug reporting if this is a debug build
#if defined( _DEBUG )
  vk::DebugUtilsMessengerCreateInfoEXT debug_message_create_info;
  debug_message_create_info.messageSeverity = 
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError 
    | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
  debug_message_create_info.messageType = 
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral 
    | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance 
    | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
  debug_message_create_info.pfnUserCallback = debugCallback;
  debug_message_create_info.pUserData = nullptr;

  vk_dispatch_loader_dynamic.init( vk_instance );
  vk::DebugUtilsMessengerEXT debug_report 
    = vk_instance.createDebugUtilsMessengerEXT( debug_message_create_info, nullptr, vk_dispatch_loader_dynamic );

  if( !debug_report ) {
    throw std::runtime_error( "Could not initialize debug reporting for vulkan!" );
  }
#endif

  vk_surface = window->createVulkanSurface( vk_instance );

  // TODO Check suitability and handle case where there is no device
  std::vector< vk::PhysicalDevice > physical_devices = vk_instance.enumeratePhysicalDevices();
  vk_physical_device = physical_devices[0];
  std::vector< vk::ExtensionProperties > physical_device_extensions 
    = vk_physical_device.enumerateDeviceExtensionProperties();
  
  std::vector< const char* > physical_device_extension_names;
  for( const auto& ext : physical_device_extensions ) {
    physical_device_extension_names.push_back( ext.extensionName );
  }

  // Find queue family with graphics support
  std::vector< vk::QueueFamilyProperties > queue_family_properties = vk_physical_device.getQueueFamilyProperties();
  graphics_queue_family_index = 0;
  for( auto& q : queue_family_properties ) {
    if( q.queueFlags & vk::QueueFlagBits::eGraphics ) {
      break;
    }
    ++graphics_queue_family_index;
  }

  float queue_priorities[] = { 1.0f };
  auto queue_info = vk::DeviceQueueCreateInfo()
    .setPQueuePriorities( queue_priorities )
    .setQueueCount( 1 )
    .setQueueFamilyIndex( graphics_queue_family_index );

  auto physical_device_features = vk::PhysicalDeviceFeatures();

  auto device_info = vk::DeviceCreateInfo()
#if defined( _DEBUG )
    .setEnabledLayerCount( static_cast< uint32_t >( layers.size() ) )
    .setPpEnabledLayerNames( layers.data() )
#endif
    .setPQueueCreateInfos( &queue_info )
    .setQueueCreateInfoCount( 1 )
    .setPEnabledFeatures( &physical_device_features )
    .setPpEnabledExtensionNames( physical_device_extension_names.data() )
    .setEnabledExtensionCount( static_cast< uint32_t >( physical_device_extension_names.size() ) );

  vk_device = vk_physical_device.createDevice( device_info );
  vk_graphics_queue = vk_device.getQueue( graphics_queue_family_index, 0 );

  // TODO Use these
  auto surface_formats = vk_physical_device.getSurfaceFormatsKHR( vk_surface );
  auto present_modes = vk_physical_device.getSurfacePresentModesKHR( vk_surface );

  createSwapchain();
  createImageViews();
  createRenderPass();
  createGraphicsPipeline();
  createSwapchainFramebuffers();
  createCommandBuffers();

}

void VulkanTest::Renderer::drawImage() {

  uint32_t image_index;
  while( true ) {
  
    // Check if the window's framebuffer size has changed
    // If it has we need to recreate the swapchain
    bool framebuffer_size_changed = false;
    const uint32_t temp_fb_width = window->getFramebufferWidth();
    const uint32_t temp_fb_height = window->getFramebufferHeight();
    if( ( temp_fb_width != window_fb_width ) || ( temp_fb_height != window_fb_height ) ) {
      framebuffer_size_changed = true;
      window_fb_width = temp_fb_width;
      window_fb_height = temp_fb_height;
    }

    /// Acquire the next available swapchain image that we can write to
    vk::Result result = vk_device.acquireNextImageKHR( 
      vk_swapchain,
      std::numeric_limits< uint32_t >::max(),
      vk_image_available_semaphore,
      nullptr,
      &image_index );

    // If the window size has changed or the image view is out of date according to Vulkan
    // then recreate the pipeline from the swapchain stage
    if( result == vk::Result::eErrorOutOfDateKHR || framebuffer_size_changed ) {
      cleanupSwapchain();
      createSwapchain();
      createImageViews();
      createRenderPass();
      createGraphicsPipeline();
      createSwapchainFramebuffers();
      createCommandBuffers();
      continue;
    } else if ( result != vk::Result::eSuccess &&
                result != vk::Result::eSuboptimalKHR ) {
      throw std::runtime_error( "Failed to acquire image!" );
    } else {
      break;
    }

  }

  // Submit commands to the queue
  vk::Semaphore wait_semaphores[] = { vk_image_available_semaphore };
  vk::Semaphore signal_semaphores[] = { vk_rendering_finished_semaphore };
  vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
  auto submit_info = vk::SubmitInfo()
    .setWaitSemaphoreCount( 1 )
    .setPWaitSemaphores( wait_semaphores )
    .setPWaitDstStageMask( wait_stages )
    .setCommandBufferCount( 1 )
    .setPCommandBuffers( &vk_command_buffers[image_index] )
    .setSignalSemaphoreCount( 1 )
    .setPSignalSemaphores( signal_semaphores );

  vk_graphics_queue.submit( submit_info, nullptr );

  vk::SwapchainKHR swapchains[] = { vk_swapchain };
  auto present_info = vk::PresentInfoKHR()
    .setWaitSemaphoreCount( 1 )
    .setPWaitSemaphores( signal_semaphores )
    .setSwapchainCount( 1 )
    .setPSwapchains( swapchains )
    .setPImageIndices( &image_index );

  vk_graphics_queue.presentKHR( present_info );

  // Wait for commands to finish
  // Todo allow writing to next available backbuffer if available
  vk_graphics_queue.waitIdle();

}

vk::Device& VulkanTest::Renderer::getDevice() {
  return vk_device;
};

void VulkanTest::Renderer::createSwapchain() {

  auto swapchain_info = vk::SwapchainCreateInfoKHR()
    .setSurface( vk_surface )
    .setMinImageCount( 3 )
    .setImageFormat( vk::Format::eB8G8R8A8Unorm )
    .setImageColorSpace( vk::ColorSpaceKHR::eSrgbNonlinear )
    .setImageExtent( { window->getWidth(), window->getHeight() } )
    .setImageArrayLayers( 1 )
    .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )
    .setImageSharingMode( vk::SharingMode::eExclusive )
    .setQueueFamilyIndexCount( 0 )
    .setPQueueFamilyIndices( nullptr )
    .setPreTransform( vk::SurfaceTransformFlagBitsKHR::eIdentity )
    .setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )
    .setPresentMode( vk::PresentModeKHR::eMailbox )
    .setClipped( true )
    .setOldSwapchain( nullptr );

  vk_swapchain = vk_device.createSwapchainKHR( swapchain_info );
  vk_swapchain_images = vk_device.getSwapchainImagesKHR( vk_swapchain );

}

void VulkanTest::Renderer::createImageViews() {

  vk_image_views.resize( vk_swapchain_images.size() );

  for( size_t i = 0; i < vk_image_views.size(); ++i ) {

    auto image_view_info = vk::ImageViewCreateInfo()
      .setImage( vk_swapchain_images[i] )
      .setViewType( vk::ImageViewType::e2D )
      .setFormat( vk::Format::eB8G8R8A8Unorm )
      .setComponents( vk::ComponentMapping( 
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity ) )
      .setSubresourceRange( vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 ) );

    vk_image_views[i] = vk_device.createImageView( image_view_info );

  }

}

void VulkanTest::Renderer::createRenderPass() {

  auto attachment_description = vk::AttachmentDescription()
    .setFormat( vk::Format::eB8G8R8A8Unorm )
    .setSamples( vk::SampleCountFlagBits::e1 )
    .setLoadOp( vk::AttachmentLoadOp::eClear )
    .setStoreOp( vk::AttachmentStoreOp::eStore )
    .setStencilLoadOp( vk::AttachmentLoadOp::eDontCare )
    .setStencilStoreOp( vk::AttachmentStoreOp::eDontCare )
    .setInitialLayout( vk::ImageLayout::eUndefined )
    .setFinalLayout( vk::ImageLayout::ePresentSrcKHR );

  auto attachment_reference = vk::AttachmentReference()
    .setAttachment( 0 )
    .setLayout( vk::ImageLayout::eColorAttachmentOptimal );

  auto subpass_description = vk::SubpassDescription()
    .setPipelineBindPoint( vk::PipelineBindPoint::eGraphics )
    .setColorAttachmentCount( 1 )
    .setPColorAttachments( &attachment_reference );

  auto subpass_dependency = vk::SubpassDependency()
    .setSrcSubpass( VK_SUBPASS_EXTERNAL )
    .setDstSubpass( 0 )
    .setSrcStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
    .setSrcAccessMask( vk::AccessFlagBits::eColorAttachmentRead )
    .setDstStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput )
    .setDstAccessMask( vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite );

  auto render_pass_info = vk::RenderPassCreateInfo()
    .setAttachmentCount( 1 )
    .setPAttachments( &attachment_description )
    .setSubpassCount( 1 )
    .setPSubpasses( &subpass_description )
    .setDependencyCount( 1 )
    .setPDependencies( &subpass_dependency );

  vk_render_pass = vk_device.createRenderPass( render_pass_info );

}

void VulkanTest::Renderer::createGraphicsPipeline() {

  std::shared_ptr< ShaderModule > fragment_shader( new ShaderModule( "C:/Users/Michael/Desktop/VK/VulkanTest/shaders/frag.spv", vk::ShaderStageFlagBits::eFragment ) );
  std::shared_ptr< ShaderModule > vertex_shader( new ShaderModule( "C:/Users/Michael/Desktop/VK/VulkanTest/shaders/vert.spv", vk::ShaderStageFlagBits::eVertex ) );
  std::vector< std::shared_ptr< ShaderModule > > shader_modules;
  shader_modules.push_back( vertex_shader );
  shader_modules.push_back( fragment_shader );
  shader.reset( new Shader( shader_modules ) );

  std::vector< Eigen::Matrix< float, 3, 1 > > data = { 
    { 0.0f, -0.5f, 0.0f },
    { 0.5f, 0.5f, 0.0f },
    { -0.5f, 0.5f, 0.0f } 
  };

  std::shared_ptr< VertexAttribute< float, 3, 1 > > position( new VertexAttribute< float, 3, 1 >( data ) );

  auto position_description = vk::VertexInputAttributeDescription()
    .setBinding( 0 )
    .setLocation( 0 )
    .setFormat( vk::Format::eR32G32B32Sfloat )
    .setOffset( 0 );

  auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo()
    .setPVertexBindingDescriptions( &position->getVkVertexInputBindingDescription() )
    .setVertexBindingDescriptionCount( 0 )
    .setPVertexAttributeDescriptions( nullptr )
    .setVertexAttributeDescriptionCount( 0 );

  auto input_assembly_info = vk::PipelineInputAssemblyStateCreateInfo()
    .setPrimitiveRestartEnable( false )
    .setTopology( vk::PrimitiveTopology::eTriangleList );

  auto viewport = vk::Viewport()
    .setX( 0 )
    .setY( 0 )
    .setWidth( static_cast< float >( window->getWidth() ) )
    .setHeight( static_cast< float >( window->getHeight() ) )
    .setMinDepth( 0.0f )
    .setMaxDepth( 1.0f );

  auto scissor = vk::Rect2D()
    .setOffset( vk::Offset2D( 0, 0 ) )
    .setExtent( vk::Extent2D( window->getWidth(), window->getHeight() ) );

  auto viewport_info = vk::PipelineViewportStateCreateInfo()
    .setPScissors( &scissor )
    .setScissorCount( 1 )
    .setPViewports( &viewport )
    .setViewportCount( 1 );

  auto rasterization_info = vk::PipelineRasterizationStateCreateInfo()
    .setRasterizerDiscardEnable( false )
    .setLineWidth( 1.0f )
    .setPolygonMode( vk::PolygonMode::eFill )
    .setCullMode( vk::CullModeFlagBits::eBack )
    .setFrontFace( vk::FrontFace::eClockwise )
    .setDepthBiasEnable( false );

  auto multisampling_info = vk::PipelineMultisampleStateCreateInfo()
    .setSampleShadingEnable( false )
    .setRasterizationSamples( vk::SampleCountFlagBits::e1 );

  auto colorblend_attachment_info = vk::PipelineColorBlendAttachmentState()
    .setColorWriteMask( 
        vk::ColorComponentFlagBits::eR 
      | vk::ColorComponentFlagBits::eG
      | vk::ColorComponentFlagBits::eB
      | vk::ColorComponentFlagBits::eA )
    .setBlendEnable( false )
    .setSrcColorBlendFactor( vk::BlendFactor::eOne )
    .setDstColorBlendFactor( vk::BlendFactor::eZero )
    .setColorBlendOp( vk::BlendOp::eAdd )
    .setSrcAlphaBlendFactor( vk::BlendFactor::eOne )
    .setDstAlphaBlendFactor( vk::BlendFactor::eZero )
    .setAlphaBlendOp( vk::BlendOp::eAdd );

  auto colorblend_info = vk::PipelineColorBlendStateCreateInfo()
    .setLogicOpEnable( false )
    .setAttachmentCount( 1 )
    .setPAttachments( &colorblend_attachment_info );

  auto layout_info = vk::PipelineLayoutCreateInfo()
    .setSetLayoutCount( 0 )
    .setPSetLayouts( nullptr )
    .setPushConstantRangeCount( 0 )
    .setPPushConstantRanges( nullptr );

  vk_layout = vk_device.createPipelineLayout( layout_info );

  auto graphics_pipeline_info = vk::GraphicsPipelineCreateInfo()
    .setStageCount( static_cast< uint32_t >( shader->getVkShaderStages().size() ) )
    .setPStages( shader->getVkShaderStages().data() )
    .setPVertexInputState( &vertex_input_info )
    .setPInputAssemblyState( &input_assembly_info )
    .setPViewportState( &viewport_info )
    .setPRasterizationState( &rasterization_info )
    .setPMultisampleState( &multisampling_info )
    .setPDepthStencilState( nullptr )
    .setPColorBlendState( &colorblend_info )
    .setPDynamicState( nullptr )
    .setLayout( vk_layout )
    .setRenderPass( vk_render_pass )
    .setSubpass( 0 );

  vk_graphics_pipeline = vk_device.createGraphicsPipeline( nullptr, graphics_pipeline_info );

}

void VulkanTest::Renderer::createSwapchainFramebuffers() {

  vk_swapchain_framebuffers.resize( vk_image_views.size() );
  for( size_t i = 0; i < vk_image_views.size(); ++i ) {
    vk::ImageView attachments [] = { vk_image_views[i] };

    auto framebuffer_info = vk::FramebufferCreateInfo()
      .setRenderPass( vk_render_pass )
      .setAttachmentCount( 1 )
      .setPAttachments( attachments )
      .setWidth( window->getWidth() )
      .setHeight( window->getHeight() )
      .setLayers( 1 );

    vk_swapchain_framebuffers[i] = vk_device.createFramebuffer( framebuffer_info );
  }

}

void VulkanTest::Renderer::createCommandBuffers() {

  auto command_pool_info = vk::CommandPoolCreateInfo()
    .setQueueFamilyIndex( graphics_queue_family_index );

  vk_command_pool = vk_device.createCommandPool( command_pool_info );

  auto command_buffer_allocate_info = vk::CommandBufferAllocateInfo()
    .setCommandBufferCount( static_cast< uint32_t >( vk_swapchain_framebuffers.size() ) )
    .setCommandPool( vk_command_pool )
    .setLevel( vk::CommandBufferLevel::ePrimary );

  vk_command_buffers = vk_device.allocateCommandBuffers( command_buffer_allocate_info );

  const std::array< float, 4 > clear_color_array = { 0.0f, 0.0f, 0.0f, 1.0f };
  auto clear_color = vk::ClearValue()
    .setColor( vk::ClearColorValue( clear_color_array ) );

  for( size_t i = 0; i < vk_command_buffers.size(); ++i ) {

    auto begin_info = vk::CommandBufferBeginInfo()
      .setFlags( vk::CommandBufferUsageFlagBits::eSimultaneousUse )
      .setPInheritanceInfo( nullptr );

    vk_command_buffers[i].begin( begin_info );

    auto render_pass_info = vk::RenderPassBeginInfo()
      .setRenderPass( vk_render_pass )
      .setFramebuffer( vk_swapchain_framebuffers[i] )
      .setRenderArea( vk::Rect2D( { 0, 0 }, { window->getWidth(), window->getHeight() } ) )
      .setClearValueCount( 1 )
      .setPClearValues( &clear_color );
    
    vk_command_buffers[i].beginRenderPass( render_pass_info, vk::SubpassContents::eInline );
    vk_command_buffers[i].bindPipeline( vk::PipelineBindPoint::eGraphics, vk_graphics_pipeline );

    vk_command_buffers[i].draw( 3, 1, 0, 0 );

    vk_command_buffers[i].endRenderPass();
    vk_command_buffers[i].end();

    auto semaphore_info = vk::SemaphoreCreateInfo();
    vk_image_available_semaphore = vk_device.createSemaphore( semaphore_info );
    vk_rendering_finished_semaphore = vk_device.createSemaphore( semaphore_info );

  }

}

void VulkanTest::Renderer::cleanup() {

  cleanupSwapchain();

  vk_device.destroySemaphore( vk_image_available_semaphore );
  vk_device.destroySemaphore( vk_rendering_finished_semaphore );
  vk_device.destroyCommandPool( vk_command_pool );

  vk_device.destroy();
#if defined( _DEBUG )
  vk_instance.destroyDebugUtilsMessengerEXT( vk_debug_utils_messenger, nullptr, vk_dispatch_loader_dynamic );
#endif
  vk_instance.destroySurfaceKHR( vk_surface );
  vk_instance.destroy();

}

void VulkanTest::Renderer::cleanupSwapchain() {

  for( size_t i = 0; i < vk_swapchain_framebuffers.size(); ++i ) {
    vk_device.destroyFramebuffer( vk_swapchain_framebuffers[i] );
  }
  vk_device.freeCommandBuffers( vk_command_pool, vk_command_buffers );
  vk_device.destroyPipeline( vk_graphics_pipeline );
  vk_device.destroyPipelineLayout( vk_layout );
  vk_device.destroyRenderPass( vk_render_pass );
  for( size_t i = 0; i < vk_image_views.size(); ++i ) {
    vk_device.destroyImageView( vk_image_views[i] );
  }
  vk_device.destroySwapchainKHR( vk_swapchain );

}