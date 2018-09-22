#include <VulkanTest/VulkanManager.h>
#include <VulkanTest/OBJLoader.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

VulkanTest::VulkanManager::VulkanManager() : frames_in_flight( 2 ), current_frame( 0 ) {
}

VulkanTest::VulkanManager::~VulkanManager() {
  vk_device.waitIdle();
  position.reset();
  index.reset();
  shader.reset();
  cleanup();
}

std::shared_ptr< VulkanTest::VulkanManager >& VulkanTest::VulkanManager::getInstance() {
  static std::shared_ptr< VulkanTest::VulkanManager > singleton_vulkan_manager_instance( new VulkanManager() );
  return singleton_vulkan_manager_instance;
}

void VulkanTest::VulkanManager::initialize( const std::shared_ptr< Window >& _window ) {

  window = _window;

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
    .setApiVersion( VK_API_VERSION_1_0 );

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

  auto physical_device_features = vk::PhysicalDeviceFeatures().
    setFragmentStoresAndAtomics( VK_TRUE );

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
  
  VmaAllocatorCreateInfo vma_allocator_create_info = {};
  vma_allocator_create_info.device = vk_device;
  vma_allocator_create_info.physicalDevice = vk_physical_device;

  if( vmaCreateAllocator( &vma_allocator_create_info, &vma_allocator ) != VK_SUCCESS ) {
    throw std::runtime_error( "Failed to create VmaAllocator!" );
  }
  
  vk_graphics_queue = vk_device.getQueue( graphics_queue_family_index, 0 );

  // TODO Use these
  auto surface_formats = vk_physical_device.getSurfaceFormatsKHR( vk_surface );
  auto present_modes = vk_physical_device.getSurfacePresentModesKHR( vk_surface );
  auto surface_support = vk_physical_device.getSurfaceSupportKHR( graphics_queue_family_index, vk_surface );

  createSwapchain();
  createImageViews();
  createRenderPass();
  createGraphicsPipeline();
  createSwapchainFramebuffers();
  createCommandBuffers();
  createSyncObjects();

}

void VulkanTest::VulkanManager::drawImage() {

  auto fence_result = vk_device.waitForFences( vk_in_flight_fences[current_frame], VK_TRUE, std::numeric_limits< uint32_t >::max() );

  uint32_t image_index;
  while( true ) {

    /// Acquire the next available swapchain image that we can write to
    vk::Result result = vk_device.acquireNextImageKHR( 
      vk_swapchain,
      std::numeric_limits< uint32_t >::max(),
      vk_image_available_semaphores[current_frame],
      nullptr,
      &image_index );

    // If the window size has changed or the image view is out of date according to Vulkan
    // then recreate the pipeline from the swapchain stage
    if( result == vk::Result::eErrorOutOfDateKHR || window->sizeHasChanged() ) {
      cleanupSwapchain();
      createSwapchain();
      createImageViews();
      createRenderPass();
      createGraphicsPipeline();
      createSwapchainFramebuffers();
      createCommandBuffers();
      createSyncObjects();
      continue;
    } else if ( result != vk::Result::eSuccess &&
                result != vk::Result::eSuboptimalKHR ) {
      throw std::runtime_error( "Failed to acquire image!" );
    } else {
      break;
    }

  }

  //// Submit commands to the queue
  vk::Semaphore wait_semaphores[] = { vk_image_available_semaphores[current_frame] };
  vk::Semaphore signal_semaphores[] = { vk_rendering_finished_semaphores[current_frame] };
  vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
  auto submit_info = vk::SubmitInfo()
    .setWaitSemaphoreCount( 1 )
    .setPWaitSemaphores( wait_semaphores )
    .setPWaitDstStageMask( wait_stages )
    .setCommandBufferCount( 1 )
    .setPCommandBuffers( &vk_command_buffers[image_index] )
    .setSignalSemaphoreCount( 1 )
    .setPSignalSemaphores( signal_semaphores );

  vk_device.resetFences( vk_in_flight_fences[current_frame] );

  /// Todo, why are there six fences?
  vk_graphics_queue.submit( submit_info, vk_in_flight_fences[current_frame] );

  vk::SwapchainKHR swapchains[] = { vk_swapchain };
  auto present_info = vk::PresentInfoKHR()
    .setWaitSemaphoreCount( 1 )
    .setPWaitSemaphores( signal_semaphores )
    .setSwapchainCount( 1 )
    .setPSwapchains( swapchains )
    .setPImageIndices( &image_index );

  auto present_result = vk_graphics_queue.presentKHR( present_info );

  current_frame = ( current_frame + 1 ) % frames_in_flight;

}

const vk::Device& VulkanTest::VulkanManager::getVkDevice() {
  return vk_device;
};

const vk::PhysicalDevice& VulkanTest::VulkanManager::getVKPhysicalDevice() {
  return vk_physical_device;
};

const vk::CommandPool& VulkanTest::VulkanManager::getVkCommandPool() {
  return vk_command_pool;
}

const vk::Queue& VulkanTest::VulkanManager::getVkGraphicsQueue() {
  return vk_graphics_queue;
}

const VmaAllocator& VulkanTest::VulkanManager::getVmaAllocator() {
  return vma_allocator;
}

void VulkanTest::VulkanManager::createSwapchain() {

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
    .setClipped( VK_TRUE )
    .setOldSwapchain( nullptr );

  vk_swapchain = vk_device.createSwapchainKHR( swapchain_info );
  vk_swapchain_images = vk_device.getSwapchainImagesKHR( vk_swapchain );

}

void VulkanTest::VulkanManager::createImageViews() {

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

void VulkanTest::VulkanManager::createRenderPass() {

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

  VkSubpassDependency dependency = {};
          dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
          dependency.dstSubpass = 0;
          dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
          dependency.srcAccessMask = 0;
          dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
          dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  auto render_pass_info = vk::RenderPassCreateInfo()
    .setAttachmentCount( 1 )
    .setPAttachments( &attachment_description )
    .setSubpassCount( 1 )
    .setPSubpasses( &subpass_description )
    .setDependencyCount( 1 )
    .setPDependencies( &static_cast< vk::SubpassDependency >( dependency ) );

  vk_render_pass = vk_device.createRenderPass( render_pass_info );

}

void VulkanTest::VulkanManager::createGraphicsPipeline() {

  std::shared_ptr< ShaderModule > fragment_shader( new ShaderModule( "C:/Users/Michael/Desktop/VK/VulkanTest/shaders/frag.spv", vk::ShaderStageFlagBits::eFragment ) );
  std::shared_ptr< ShaderModule > vertex_shader( new ShaderModule( "C:/Users/Michael/Desktop/VK/VulkanTest/shaders/vert.spv", vk::ShaderStageFlagBits::eVertex ) );
  std::vector< std::shared_ptr< ShaderModule > > shader_modules;
  shader_modules.push_back( vertex_shader );
  shader_modules.push_back( fragment_shader );
  shader.reset( new Shader( shader_modules ) );

  camera.reset( new Camera< float >() );

  uniform_buffers.resize( vk_swapchain_images.size() );
  for( auto& ub : uniform_buffers ) {
    ub.reset( new UniformBuffer< UniformBufferObject >( 0 ) );
  }

  auto meshes = VulkanTest::loadOBJ( "C:/Users/Michael/Desktop/VK/VulkanTest/models/teapot.obj", "" );
  mesh = meshes[0];

  std::vector< uint16_t > index_data = { 0, 1, 2 };

  index.reset( new IndexAttribute< uint16_t >( index_data.data(), index_data.size() ) );

  std::vector< Eigen::Matrix< float, 3, 1 > > data = { 
    { 0.0f, -0.5f, 0 },
    { 0.5f, 0.5f, 0 },
    { -0.5f, 0.5f, 0 } 
  };

  position.reset( new VertexAttribute< Eigen::Vector3f >( data.data(), data.size(), 0, vk::Format::eR32G32B32Sfloat ) );

  auto position_description = vk::VertexInputAttributeDescription()
    .setBinding( 0 )
    .setLocation( 0 )
    .setFormat( vk::Format::eR32G32B32Sfloat )
    .setOffset( 0 );

  auto a = vk::VertexInputBindingDescription()
    .setBinding( 0 )
    .setInputRate( vk::VertexInputRate::eVertex )
    .setStride( sizeof( Eigen::Vector3f ) );

  auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo()
    .setPVertexBindingDescriptions( &a )
    .setVertexBindingDescriptionCount( 1 )
    .setPVertexAttributeDescriptions( &position_description )
    .setVertexAttributeDescriptionCount( 1 );

  auto input_assembly_info = vk::PipelineInputAssemblyStateCreateInfo()
    .setPrimitiveRestartEnable( VK_FALSE )
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
    .setRasterizerDiscardEnable( VK_FALSE )
    .setLineWidth( 1.0f )
    .setPolygonMode( vk::PolygonMode::eFill )
    .setCullMode( vk::CullModeFlagBits::eNone )
    .setFrontFace( vk::FrontFace::eCounterClockwise )
    .setDepthBiasEnable( VK_FALSE );

  auto multisampling_info = vk::PipelineMultisampleStateCreateInfo()
    .setSampleShadingEnable( VK_FALSE )
    .setRasterizationSamples( vk::SampleCountFlagBits::e1 );

  auto colorblend_attachment_info = vk::PipelineColorBlendAttachmentState()
    .setColorWriteMask( 
        vk::ColorComponentFlagBits::eR 
      | vk::ColorComponentFlagBits::eG
      | vk::ColorComponentFlagBits::eB
      | vk::ColorComponentFlagBits::eA )
    .setBlendEnable( VK_FALSE )
    .setSrcColorBlendFactor( vk::BlendFactor::eOne )
    .setDstColorBlendFactor( vk::BlendFactor::eZero )
    .setColorBlendOp( vk::BlendOp::eAdd )
    .setSrcAlphaBlendFactor( vk::BlendFactor::eOne )
    .setDstAlphaBlendFactor( vk::BlendFactor::eZero )
    .setAlphaBlendOp( vk::BlendOp::eAdd );

  auto colorblend_info = vk::PipelineColorBlendStateCreateInfo()
    .setLogicOpEnable( VK_FALSE )
    .setAttachmentCount( 1 )
    .setPAttachments( &colorblend_attachment_info );

  auto pool_size = vk::DescriptorPoolSize()
    .setDescriptorCount( static_cast< uint32_t >( vk_swapchain_images.size() ) )
    .setType( vk::DescriptorType::eUniformBuffer );

  auto pool_create_info = vk::DescriptorPoolCreateInfo()
    .setPoolSizeCount( 1 )
    .setPPoolSizes( &pool_size )
    .setMaxSets( static_cast< uint32_t >( vk_swapchain_images.size() ) );

  vk_descriptor_pool = vk_device.createDescriptorPool( pool_create_info );

  auto descriptor_set_info = vk::DescriptorSetLayoutCreateInfo()
    .setBindingCount( 1 )
    .setPBindings( &uniform_buffers[0]->getVkDescriptorSetLayoutBinding() );

  auto descriptor_set = vk_device.createDescriptorSetLayout( descriptor_set_info );

  auto layout_info = vk::PipelineLayoutCreateInfo()
    .setSetLayoutCount( 1 )
    .setPSetLayouts( &descriptor_set )
    .setPushConstantRangeCount( 0 )
    .setPPushConstantRanges( nullptr );

  std::vector< vk::DescriptorSetLayout > descriptor_set_layouts( vk_swapchain_images.size(), descriptor_set );
  auto descriptor_set_allocate_info = vk::DescriptorSetAllocateInfo()
    .setDescriptorPool( vk_descriptor_pool )
    .setDescriptorSetCount( static_cast< uint32_t >( vk_swapchain_images.size() ) )
    .setPSetLayouts( descriptor_set_layouts.data() );

  vk_descriptor_sets = vk_device.allocateDescriptorSets( descriptor_set_allocate_info );

  for( size_t i = 0; i < uniform_buffers.size(); ++i ) {

    auto descriptor_buffer_info = vk::DescriptorBufferInfo()
      .setBuffer( uniform_buffers[i]->getVkBuffer() )
      .setOffset( 0 )
      .setRange( sizeof( UniformBufferObject ) );

    auto write_descriptor_set = vk::WriteDescriptorSet()
      .setDstBinding( 0 )
      .setDstArrayElement( 0 )
      .setDstSet( vk_descriptor_sets[i] )
      .setDescriptorType( vk::DescriptorType::eUniformBuffer )
      .setDescriptorCount( 1 )
      .setPBufferInfo( &descriptor_buffer_info );

    vk_device.updateDescriptorSets( write_descriptor_set, nullptr );

  }

  vk_layout = vk_device.createPipelineLayout( layout_info );

  auto graphics_pipeline_info = vk::GraphicsPipelineCreateInfo()
    .setStageCount( static_cast< uint32_t >( shader->getVkShaderStages().size() ) )
    .setPStages( shader->getVkShaderStages().data() )
    .setPVertexInputState( &mesh->getVkPipelineVertexInputStateCreateInfo() )
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

void VulkanTest::VulkanManager::createSwapchainFramebuffers() {

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

void VulkanTest::VulkanManager::createCommandBuffers() {

  auto command_pool_info = vk::CommandPoolCreateInfo()
    .setQueueFamilyIndex( graphics_queue_family_index );

  vk_command_pool = vk_device.createCommandPool( command_pool_info );

  mesh->transferBuffers();

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

    UniformBufferObject vp_data;
    vp_data.projection = camera->getPerspectiveProjectionMatrix();
    vp_data.view = camera->getViewMatrix();
    for( auto& ub : uniform_buffers ) {
      ub->updateUniformBuffer( &vp_data, 1 );
    }

    std::vector< vk::Buffer > buffers = { position->getVkBuffer() };
    //vk_command_buffers[i].bindVertexBuffers( 0, buffers, { 0 } );
    //vk_command_buffers[i].bindIndexBuffer( index->getVkBuffer(), 0, vk::IndexType::eUint16 );
    mesh->bindVertexBuffers( vk_command_buffers[i] );
    mesh->bindIndexBuffer( vk_command_buffers[i] );

    vk_command_buffers[i].bindDescriptorSets( vk::PipelineBindPoint::eGraphics, vk_layout, 0, vk_descriptor_sets[i], nullptr );

    mesh->draw( vk_command_buffers[i] );
    //vk_command_buffers[i].drawIndexed( static_cast< uint32_t >( index->getNumElements() ), 1, 0, 0, 0 );

    vk_command_buffers[i].endRenderPass();
    vk_command_buffers[i].end();

  }

}

void VulkanTest::VulkanManager::createSyncObjects() {

  auto semaphore_info = vk::SemaphoreCreateInfo();
  auto fence_info = vk::FenceCreateInfo()
    .setFlags( vk::FenceCreateFlagBits::eSignaled );
  for( size_t i = 0; i < frames_in_flight; ++i ) {
    vk_image_available_semaphores.push_back( vk_device.createSemaphore( semaphore_info ) );
    vk_rendering_finished_semaphores.push_back( vk_device.createSemaphore( semaphore_info ) );
    vk_in_flight_fences.push_back( vk_device.createFence( fence_info ) );
    if( !vk_image_available_semaphores.back() 
      || !vk_rendering_finished_semaphores.back() 
      || !vk_in_flight_fences.back() ) {
      throw std::runtime_error( "Could not create sync objects for rendering!" );
    }
  }

}

void VulkanTest::VulkanManager::cleanup() {

  cleanupSwapchain();

  vk_device.destroyDescriptorPool( vk_descriptor_pool );
  for( size_t i = 0; i < frames_in_flight; ++i ) {
    vk_device.destroySemaphore( vk_image_available_semaphores[i] );
    vk_device.destroySemaphore( vk_rendering_finished_semaphores[i] );  
    vk_device.destroyFence( vk_in_flight_fences[i] );
  }
  vk_device.destroyCommandPool( vk_command_pool );

  vmaDestroyAllocator( vma_allocator );

  vk_device.destroy();
#if defined( _DEBUG )
  vk_instance.destroyDebugUtilsMessengerEXT( vk_debug_utils_messenger, nullptr, vk_dispatch_loader_dynamic );
#endif
  vk_instance.destroySurfaceKHR( vk_surface );
  vk_instance.destroy();

}

void VulkanTest::VulkanManager::cleanupSwapchain() {

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

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanTest::VulkanManager::debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
  VkDebugUtilsMessageTypeFlagsEXT message_type,
  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
  void* user_data ) {
  std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
  return VK_FALSE;
}