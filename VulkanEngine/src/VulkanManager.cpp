#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/Image.h>

#include "vulkan/vulkan_core.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

VulkanEngine::VulkanManager::VulkanManager()
    : frames_in_flight(3), current_frame(0) {}

VulkanEngine::VulkanManager::~VulkanManager() {
  vk_device.waitIdle();
  cleanup();
}

std::shared_ptr<VulkanEngine::VulkanManager> &
VulkanEngine::VulkanManager::getInstance() {
  static std::shared_ptr<VulkanEngine::VulkanManager>
      singleton_vulkan_manager_instance(new VulkanManager());
  return singleton_vulkan_manager_instance;
}

void VulkanEngine::VulkanManager::destroyInstance() {
  static std::shared_ptr<VulkanEngine::VulkanManager>
      singleton_vulkan_manager_instance;
  singleton_vulkan_manager_instance.reset();
}

void VulkanEngine::VulkanManager::initialize(
    const std::shared_ptr<Window> &_window) {

  window = _window;

  std::vector<const char *> instance_extensions(
      window->getRequiredVulkanInstanceExtensions());

  // Use validation layers if this is a debug build
  std::vector<const char *> layers;

#ifdef ENABLE_VULKAN_VALIDATION
  layers.push_back("VK_LAYER_KHRONOS_validation");
  instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  instance_extensions.push_back("VK_KHR_get_physical_device_properties2");
  instance_extensions.push_back("VK_KHR_device_group_creation");
  instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

  instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
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
          .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
          .setPApplicationInfo(&app_info)
          .setEnabledExtensionCount(
              static_cast<uint32_t>(instance_extensions.size()))
          .setPpEnabledExtensionNames(instance_extensions.data())
          .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
          .setPpEnabledLayerNames(layers.data());

  vk_instance = vk::createInstance(inst_info);

#ifdef ENABLE_VULKAN_VALIDATION
  vk::DebugUtilsMessengerCreateInfoEXT debug_message_create_info;
  debug_message_create_info.messageSeverity =
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
  debug_message_create_info.messageType =
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
  debug_message_create_info.pfnUserCallback = debugCallback;
  debug_message_create_info.pUserData = nullptr;

  vk_dispatch_loader_dynamic.init(vk_instance, vkGetInstanceProcAddr, vk_device,
                                  vkGetDeviceProcAddr);
  vk_debug_utils_messenger = vk_instance.createDebugUtilsMessengerEXT(
      debug_message_create_info, nullptr, vk_dispatch_loader_dynamic);

  if (!vk_debug_utils_messenger) {
    throw std::runtime_error(
        "Could not initialize debug reporting for vulkan!");
  }
#endif

  vk_surface = window->createVkSurface(vk_instance);

  // TODO Check suitability and handle case where there is no device
  std::vector<vk::PhysicalDevice> physical_devices =
      vk_instance.enumeratePhysicalDevices();
  vk_physical_device = physical_devices[0];
  std::vector<vk::ExtensionProperties> physical_device_extensions =
      vk_physical_device.enumerateDeviceExtensionProperties();

  std::vector<const char *> physical_device_extension_names;
  for (const auto &ext : physical_device_extensions) {
    physical_device_extension_names.push_back(ext.extensionName);
  }

  physical_device_extension_names.push_back(
      VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME);
  physical_device_extension_names.push_back(
      VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);

  bool found_VK_KHR_maintenance1 = false;
  bool found_VK_AMD_negative_viewport_height = false;
  for (auto it = physical_device_extension_names.begin();
       it != physical_device_extension_names.end(); ++it) {

    if (!(found_VK_KHR_maintenance1 && found_VK_AMD_negative_viewport_height)) {
      if (std::string(*it) == "VK_KHR_maintenance1") {
        found_VK_KHR_maintenance1 = true;
      } else if (std::string(*it) == "VK_AMD_negative_viewport_height") {
        found_VK_AMD_negative_viewport_height = true;
      }
    }

    if (found_VK_KHR_maintenance1 && found_VK_AMD_negative_viewport_height) {
      std::cout << "Both VK_KHR_maintenance1 and "
                   "VK_AMD_negative_viewport_height were found in the "
                << "list of physical device extensions. Removed "
                   "VK_AMD_negative_viewport_height since enabling "
                << "both is not allowed by the Vulkan specification."
                << std::endl;
      physical_device_extension_names.erase(it);
      break;
    }
  }

  // Find queue family with graphics support
  std::vector<vk::QueueFamilyProperties> queue_family_properties =
      vk_physical_device.getQueueFamilyProperties();
  graphics_queue_family_index = 0;
  for (auto &q : queue_family_properties) {
    if (q.queueFlags & vk::QueueFlagBits::eGraphics) {
      break;
    }
    ++graphics_queue_family_index;
  }

  float queue_priorities[] = {1.0f};
  auto queue_info = vk::DeviceQueueCreateInfo()
                        .setPQueuePriorities(queue_priorities)
                        .setQueueCount(1)
                        .setQueueFamilyIndex(graphics_queue_family_index);

  auto physical_device_features = vk::PhysicalDeviceFeatures()
                                      .setSamplerAnisotropy(VK_TRUE)
                                      .setFragmentStoresAndAtomics(VK_TRUE)
                                      .setSampleRateShading(VK_TRUE);

  auto device_info =
      vk::DeviceCreateInfo()
          .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
          .setPpEnabledLayerNames(layers.data())
          .setPQueueCreateInfos(&queue_info)
          .setQueueCreateInfoCount(1)
          .setPEnabledFeatures(&physical_device_features)
          .setPpEnabledExtensionNames(physical_device_extension_names.data())
          .setEnabledExtensionCount(
              static_cast<uint32_t>(physical_device_extension_names.size()));

  /// TODO Device class which holds queue, allocator and device info
  vk_device = vk_physical_device.createDevice(device_info);

  VmaAllocatorCreateInfo vma_allocator_create_info = {};
  vma_allocator_create_info.device = vk_device;
  vma_allocator_create_info.physicalDevice = vk_physical_device;
  vma_allocator_create_info.instance = vk_instance;

  if (vmaCreateAllocator(&vma_allocator_create_info, &vma_allocator) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create VmaAllocator!");
  }

  vk_graphics_queue = vk_device.getQueue(graphics_queue_family_index, 0);

  // TODO Use these
  auto surface_formats = vk_physical_device.getSurfaceFormatsKHR(vk_surface);
  auto present_modes = vk_physical_device.getSurfacePresentModesKHR(vk_surface);
  auto surface_support = vk_physical_device.getSurfaceSupportKHR(
      graphics_queue_family_index, vk_surface);

  createCommandPool();
  createSwapchain();
  createImageViews();

  default_render_pass.reset(new RenderPass(window->getFramebufferWidth(), window->getFramebufferHeight()));

  //createRenderPass();
  createSwapchainFramebuffers();
  createSyncObjects();
  createCommandBuffers();
}

void VulkanEngine::VulkanManager::beginRenderPass() {

  const std::array<float, 4> clear_color_array = {0.0f, 0.0f, 0.0f, 1.0f};
  auto clear_color =
      vk::ClearValue().setColor(vk::ClearColorValue(clear_color_array));
  auto clear_depth_stencil =
      vk::ClearValue().setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0));

  std::array<vk::ClearValue, 3> clear_values = {
      clear_color, clear_depth_stencil, clear_color};

  auto begin_info =
      vk::CommandBufferBeginInfo()
          .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse)
          .setPInheritanceInfo(nullptr);

  auto fence_result =
      vk_device.waitForFences(vk_in_flight_fences[current_frame], VK_TRUE,
                              std::numeric_limits<uint32_t>::max());
  if (fence_result != vk::Result::eSuccess) {
    throw std::runtime_error("Error waiting for fences");
  }

  vk_command_buffers[current_frame].begin(begin_info);

  auto render_pass_info =
      vk::RenderPassBeginInfo()
          .setRenderPass(default_render_pass->getVkRenderPass())
          .setFramebuffer(vk_swapchain_framebuffers[current_frame])
          .setRenderArea(vk::Rect2D({0, 0}, {window->getFramebufferWidth(),
                                             window->getFramebufferHeight()}))
          .setClearValueCount(static_cast<uint32_t>(clear_values.size()))
          .setPClearValues(clear_values.data());

  vk_command_buffers[current_frame].beginRenderPass(
      render_pass_info, vk::SubpassContents::eInline);
}

void VulkanEngine::VulkanManager::endRenderPass() {

  vk_command_buffers[current_frame].endRenderPass();
  vk_command_buffers[current_frame].end();
}

vk::CommandBuffer VulkanEngine::VulkanManager::getCurrentCommandBuffer() {
  return vk_command_buffers[current_frame];
}

void VulkanEngine::VulkanManager::drawImage() {

  // Submit commands to the queue
  if (!vk_command_buffers.empty()) {

    uint32_t image_index;
    while (true) {

      // Acquire the next available swapchain image that we can write to
      vk::Result result = vk_device.acquireNextImageKHR(
          vk_swapchain, std::numeric_limits<uint32_t>::max(),
          vk_image_available_semaphores[current_frame], nullptr, &image_index);

      // If the window size has changed or the image view is out of date
      // according to Vulkan then recreate the pipeline from the swapchain stage

      if (result == vk::Result::eErrorOutOfDateKHR || window->sizeHasChanged()) {
        vk_device.waitIdle();
        cleanupSwapchain();
        createCommandPool();
        createSwapchain();
        createImageViews();
        default_render_pass.reset(new RenderPass(window->getFramebufferWidth(), window->getFramebufferHeight()));
        createSwapchainFramebuffers();
        createSyncObjects();
        createCommandBuffers();
        current_frame = 0;
        return;
      } else if (result != vk::Result::eSuccess &&
                 result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("Failed to acquire image!");
      } else {
        break;
      }
    }

    vk::Semaphore signal_semaphores[] = {
        vk_rendering_finished_semaphores[current_frame]};
    uint32_t signal_semaphores_count = 1;

    vk::Semaphore wait_semaphores[] = {
        vk_image_available_semaphores[current_frame]};
    vk::PipelineStageFlags wait_stages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};
    auto submit_info =
        vk::SubmitInfo()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(wait_semaphores)
            .setPWaitDstStageMask(wait_stages)
            .setCommandBufferCount(1)
            .setPCommandBuffers(&vk_command_buffers[current_frame])
            .setSignalSemaphoreCount(signal_semaphores_count)
            .setPSignalSemaphores(signal_semaphores);

    vk_device.resetFences(vk_in_flight_fences[current_frame]);

    vk_graphics_queue.submit(submit_info, vk_in_flight_fences[current_frame]);

    vk::SwapchainKHR swapchains[] = {vk_swapchain};
    auto present_info = vk::PresentInfoKHR()
                            .setWaitSemaphoreCount(signal_semaphores_count)
                            .setPWaitSemaphores(signal_semaphores)
                            .setSwapchainCount(1)
                            .setPSwapchains(swapchains)
                            .setPImageIndices(&image_index);

    auto present_result = vk_graphics_queue.presentKHR(present_info);
    if (present_result != vk::Result::eSuccess) {
      throw std::runtime_error("Error presenting image to screen");
    }
  }

  current_frame = (current_frame + 1) % frames_in_flight;
}

const vk::Device &VulkanEngine::VulkanManager::getVkDevice() {
  return vk_device;
};

const vk::PhysicalDevice &VulkanEngine::VulkanManager::getVKPhysicalDevice() {
  return vk_physical_device;
};

const vk::CommandPool &VulkanEngine::VulkanManager::getVkCommandPool() {
  return vk_command_pool;
}

const vk::Queue &VulkanEngine::VulkanManager::getVkGraphicsQueue() {
  return vk_graphics_queue;
}

const VmaAllocator &VulkanEngine::VulkanManager::getVmaAllocator() {
  return vma_allocator;
}

void VulkanEngine::VulkanManager::createCommandPool() {
  vk::CommandPoolCreateInfo command_pool_info(
      vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      graphics_queue_family_index);

  vk_command_pool = vk_device.createCommandPool(command_pool_info);
}

void VulkanEngine::VulkanManager::createSwapchain() {

  auto swapchain_info =
      vk::SwapchainCreateInfoKHR()
          .setSurface(vk_surface)
          .setMinImageCount(frames_in_flight)
          .setImageFormat(vk::Format::eB8G8R8A8Unorm)
          .setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
          .setImageExtent(
              {window->getFramebufferWidth(), window->getFramebufferHeight()})
          .setImageArrayLayers(1)
          .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
          .setImageSharingMode(vk::SharingMode::eExclusive)
          .setQueueFamilyIndexCount(0)
          .setPQueueFamilyIndices(nullptr)
          .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
          .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
          .setPresentMode(vk::PresentModeKHR::eFifo)
          .setClipped(VK_TRUE)
          .setOldSwapchain(nullptr);

  vk_swapchain = vk_device.createSwapchainKHR(swapchain_info);
  vk_swapchain_images = vk_device.getSwapchainImagesKHR(vk_swapchain);
}

void VulkanEngine::VulkanManager::createImageViews() {

  vk_image_views.resize(vk_swapchain_images.size());

  for (size_t i = 0; i < vk_image_views.size(); ++i) {

    auto image_view_info =
        vk::ImageViewCreateInfo()
            .setImage(vk_swapchain_images[i])
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(vk::Format::eB8G8R8A8Unorm)
            .setComponents(
                vk::ComponentMapping(vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity,
                                     vk::ComponentSwizzle::eIdentity))
            .setSubresourceRange(vk::ImageSubresourceRange(
                vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

    vk_image_views[i] = vk_device.createImageView(image_view_info);
  }
}

void VulkanEngine::VulkanManager::createSwapchainFramebuffers() {

  /// Need a framebuffer wrapper class
  vk_swapchain_framebuffers.resize(vk_image_views.size());
  for (size_t i = 0; i < vk_image_views.size(); ++i) {
    std::array<vk::ImageView, 3> attachments = {
        default_render_pass->getColorAttachment()->getVkImageView(),
        default_render_pass->getDepthStencilAttachment()->getVkImageView(), vk_image_views[i]};

    auto framebuffer_info =
        vk::FramebufferCreateInfo()
            .setRenderPass(default_render_pass->getVkRenderPass())
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setPAttachments(attachments.data())
            .setWidth(window->getFramebufferWidth())
            .setHeight(window->getFramebufferHeight())
            .setLayers(1);

    vk_swapchain_framebuffers[i] =
        vk_device.createFramebuffer(framebuffer_info);
  }
}

void VulkanEngine::VulkanManager::createCommandBuffers() {

  auto command_buffer_allocate_info =
      vk::CommandBufferAllocateInfo()
          .setCommandBufferCount(
              static_cast<uint32_t>(vk_swapchain_framebuffers.size()))
          .setCommandPool(vk_command_pool)
          .setLevel(vk::CommandBufferLevel::ePrimary);

  vk_command_buffers =
      vk_device.allocateCommandBuffers(command_buffer_allocate_info);
}

void VulkanEngine::VulkanManager::createSyncObjects() {

  auto semaphore_info = vk::SemaphoreCreateInfo();
  auto fence_info =
      vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
  for (size_t i = 0; i < frames_in_flight; ++i) {
    vk_image_available_semaphores.push_back(
        vk_device.createSemaphore(semaphore_info));
    vk_rendering_finished_semaphores.push_back(
        vk_device.createSemaphore(semaphore_info));
    vk_in_flight_fences.push_back(vk_device.createFence(fence_info));
    if (!vk_image_available_semaphores.back() ||
        !vk_rendering_finished_semaphores.back() ||
        !vk_in_flight_fences.back()) {
      throw std::runtime_error("Could not create sync objects for rendering!");
    }
  }
}

void VulkanEngine::VulkanManager::cleanup() {

  for (size_t i = 0; i < frames_in_flight; ++i) {
    vk_device.destroySemaphore(vk_image_available_semaphores[i]);
    vk_device.destroySemaphore(vk_rendering_finished_semaphores[i]);
    vk_device.destroyFence(vk_in_flight_fences[i]);
  }

  default_render_pass.reset();

  cleanupSwapchain();

  vmaDestroyAllocator(vma_allocator);

  vk_device.destroy();
#ifdef ENABLE_VULKAN_VALIDATION
  vk_instance.destroyDebugUtilsMessengerEXT(vk_debug_utils_messenger, nullptr,
                                            vk_dispatch_loader_dynamic);
#endif
  vk_instance.destroySurfaceKHR(vk_surface);
  vk_instance.destroy();
}

void VulkanEngine::VulkanManager::cleanupSwapchain() {

  vk_device.freeCommandBuffers(vk_command_pool, vk_command_buffers);
  vk_device.destroyCommandPool(vk_command_pool);
  vk_command_buffers.clear();

  for (size_t i = 0; i < vk_swapchain_framebuffers.size(); ++i) {
    vk_device.destroyFramebuffer(vk_swapchain_framebuffers[i]);
  }
  vk_swapchain_framebuffers.clear();

  for (size_t i = 0; i < vk_image_views.size(); ++i) {
    vk_device.destroyImageView(vk_image_views[i]);
  }
  vk_image_views.clear();

  vk_device.destroySwapchainKHR(vk_swapchain);
  vk_swapchain = nullptr;
}

#ifdef ENABLE_VULKAN_VALIDATION
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngine::VulkanManager::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data) {
  std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
  return VK_FALSE;
}
#endif
