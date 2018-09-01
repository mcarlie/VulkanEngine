#include <VulkanTest/Renderer.h>

VulkanTest::Renderer::Renderer() {
}

VulkanTest::Renderer::~Renderer() {
  vk_device.destroy();
#if defined(_DEBUG)
  // TODO Crashing on exit
  //vulkan_instance.destroyDebugUtilsMessengerEXT( debug_utils_messenger, nullptr, dispatch_loader_dynamic );
#endif
  //vulkan_instance.destroySurfaceKHR( vulkan_surface );
  vk_instance.destroy();
}

VulkanTest::Renderer& VulkanTest::Renderer::getInstance() {
  static Renderer singleton_renderer_instance;
  return singleton_renderer_instance;
}

void VulkanTest::Renderer::initialize( const std::shared_ptr< Window >& _window ) {

  window = _window;

  std::vector< const char* > instance_extensions( window->getRequiredVulkanInstanceExtensions() );

  // Use validation layers if this is a debug build
  std::vector< const char* > layers;
#if defined(_DEBUG)
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
#if defined(_DEBUG)
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
  for( auto& ext : physical_device_extensions ) {
    physical_device_extension_names.push_back( ext.extensionName );
  }

  // Find queue family with graphics support
  std::vector< vk::QueueFamilyProperties > queue_family_properties = vk_physical_device.getQueueFamilyProperties();
  uint32_t graphics_queue_family_index = 0;
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
    .setEnabledExtensionCount( physical_device_extension_names.size() );

  vk_device = vk_physical_device.createDevice( device_info );
  vk_graphics_queue = vk_device.getQueue( graphics_queue_family_index, 0 );

  auto swapchain_info = vk::SwapchainCreateInfoKHR()
    .setSurface( vk_surface )
    .setMinImageCount( 2 )
    .setImageFormat( vk::Format::eR8G8B8A8Unorm )
    .setImageColorSpace( vk::ColorSpaceKHR::eSrgbNonlinear )
    .setImageExtent( { static_cast< uint32_t >( window->getWidth() ), static_cast< uint32_t >( window->getHeight() ) } )
    .setImageArrayLayers( 1 )
    .setImageUsage( vk::ImageUsageFlagBits::eColorAttachment )
    .setImageSharingMode( vk::SharingMode::eExclusive )
    .setQueueFamilyIndexCount( 0 )
    .setPQueueFamilyIndices( nullptr )
    .setPreTransform( vk::SurfaceTransformFlagBitsKHR::eIdentity )
    .setCompositeAlpha( vk::CompositeAlphaFlagBitsKHR::eOpaque )
    .setPresentMode( vk::PresentModeKHR::eImmediate )
    .setClipped( true )
    .setOldSwapchain( nullptr );

  vk_swapchain = vk_device.createSwapchainKHR( swapchain_info );
  vk_swapchain_images = vk_device.getSwapchainImagesKHR( vk_swapchain );
  vk_image_views.resize( vk_swapchain_images.size() );

  for( size_t i = 0; i < vk_image_views.size(); ++i ) {

    auto image_view_info = vk::ImageViewCreateInfo()
      .setImage( vk_swapchain_images[i] )
      .setViewType( vk::ImageViewType::e2D )
      .setFormat( vk::Format::eR8G8B8A8Unorm )
      .setComponents( vk::ComponentMapping( 
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity ) )
      .setSubresourceRange( vk::ImageSubresourceRange( vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 ) );

    vk_image_views[i] = vk_device.createImageView( image_view_info );

  }

}

void VulkanTest::Renderer::createGraphicsPipeline() {

}