#include <VulkanEngine/Framebuffer.h>
#include <VulkanEngine/Image.h>
#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/Swapchain.h>
#include <VulkanEngine/VulkanManager.h>
#include <memory>

#include "vulkan/vulkan_core.h"

#define VMA_IMPLEMENTATION
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#endif
#include <vk_mem_alloc.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

VulkanEngine::VulkanManager::VulkanManager()
    : frames_in_flight(3), current_frame(0), window(nullptr),
      initialized(false) {}

VulkanEngine::VulkanManager::~VulkanManager() { cleanup(); }

std::shared_ptr<VulkanEngine::VulkanManager> &
VulkanEngine::VulkanManager::getInstanceInternal() {
  static std::shared_ptr<VulkanManager> instance;
  if (instance.get() == nullptr) {
    instance.reset(new VulkanManager());
  }
  return instance;
}

VulkanEngine::VulkanManager &VulkanEngine::VulkanManager::getInstance() {
  return *(getInstanceInternal().get());
}

void VulkanEngine::VulkanManager::resetInstance() {
  getInstance().cleanup();
  getInstanceInternal().reset();
}

bool VulkanEngine::VulkanManager::initialize(
    const std::shared_ptr<Window> _window) {
  initialized = true;

  window = _window;

  try {
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

#ifdef __APPLE__
    instance_extensions.push_back(
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
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
#ifdef __APPLE__
            .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
#endif
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

    vk_dispatch_loader_dynamic.init(vk_instance, vkGetInstanceProcAddr,
                                    vk_device, vkGetDeviceProcAddr);
    vk_debug_utils_messenger = vk_instance.createDebugUtilsMessengerEXT(
        debug_message_create_info, nullptr, vk_dispatch_loader_dynamic);

    if (!vk_debug_utils_messenger) {
      throw std::runtime_error(
          "Could not initialize debug reporting for vulkan!");
    }
#endif

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

#ifdef __APPLE__
    physical_device_extension_names.push_back(
        VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME);
    physical_device_extension_names.push_back(
        VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
#endif

    bool found_VK_KHR_maintenance1 = false;
    bool found_VK_AMD_negative_viewport_height = false;
    for (auto it = physical_device_extension_names.begin();
         it != physical_device_extension_names.end(); ++it) {
      if (!(found_VK_KHR_maintenance1 &&
            found_VK_AMD_negative_viewport_height)) {
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
    // auto surface_formats =
    // vk_physical_device.getSurfaceFormatsKHR(vk_surface); auto present_modes =
    //     vk_physical_device.getSurfacePresentModesKHR(vk_surface);
    // auto surface_support = vk_physical_device.getSurfaceSupportKHR(
    //     graphics_queue_family_index, vk_surface);

    createCommandPool();

    default_render_pass.reset(new RenderPass(window->getFramebufferWidth(),
                                             window->getFramebufferHeight()));
    swapchain.reset(
        new Swapchain(frames_in_flight, window, default_render_pass));

    createCommandBuffers();

  } catch (const std::exception &e) {
    std::cerr << "Exception during engine initialization: " << e.what()
              << std::endl;
    return false;
  } catch (...) {
    std::cerr << "An unknown error occurred during engine initialization."
              << std::endl;
    return false;
  }

  return true;
}

vk::CommandBuffer VulkanEngine::VulkanManager::getCurrentCommandBuffer() {
  return vk_command_buffers[current_frame];
}

vk::Framebuffer VulkanEngine::VulkanManager::getCurrentSwapchainFramebuffer() {
  return swapchain->getFramebuffer(current_frame);
}

void VulkanEngine::VulkanManager::waitForFence() { swapchain->waitForFence(); }

void VulkanEngine::VulkanManager::drawImage() {
  // Submit commands to the queue
  if (!vk_command_buffers.empty()) {
    if (!swapchain->present()) {
      vk_device.waitIdle();
      cleanupCommandBuffers();
      createCommandPool();
      default_render_pass.reset(new RenderPass(window->getFramebufferWidth(),
                                               window->getFramebufferHeight()));
      swapchain.reset(
          new Swapchain(frames_in_flight, window, default_render_pass));
      createCommandBuffers();
      current_frame = 0;
      return;
    }
  }

  current_frame = (current_frame + 1) % frames_in_flight;
}

const vk::Device &VulkanEngine::VulkanManager::getVkDevice() const {
  return vk_device;
};

const vk::PhysicalDevice &
VulkanEngine::VulkanManager::getVKPhysicalDevice() const {
  return vk_physical_device;
};

const vk::CommandPool &VulkanEngine::VulkanManager::getVkCommandPool() const {
  return vk_command_pool;
}

const vk::Queue &VulkanEngine::VulkanManager::getVkGraphicsQueue() const {
  return vk_graphics_queue;
}

const VmaAllocator &VulkanEngine::VulkanManager::getVmaAllocator() const {
  return vma_allocator;
}

void VulkanEngine::VulkanManager::createCommandPool() {
  vk::CommandPoolCreateInfo command_pool_info(
      vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      graphics_queue_family_index);

  vk_command_pool = vk_device.createCommandPool(command_pool_info);
}

void VulkanEngine::VulkanManager::createCommandBuffers() {
  auto command_buffer_allocate_info =
      vk::CommandBufferAllocateInfo()
          .setCommandBufferCount(static_cast<uint32_t>(frames_in_flight))
          .setCommandPool(vk_command_pool)
          .setLevel(vk::CommandBufferLevel::ePrimary);

  vk_command_buffers =
      vk_device.allocateCommandBuffers(command_buffer_allocate_info);
}

void VulkanEngine::VulkanManager::cleanup() {
  if (!initialized) {
    return;
  }

  vk_device.waitIdle();
  default_render_pass.reset();
  swapchain.reset();

  cleanupCommandBuffers();

  vmaDestroyAllocator(vma_allocator);
  vma_allocator = nullptr;

  vk_device.destroy();
#ifdef ENABLE_VULKAN_VALIDATION
  vk_instance.destroyDebugUtilsMessengerEXT(vk_debug_utils_messenger, nullptr,
                                            vk_dispatch_loader_dynamic);
#endif
  window.reset();
  vk_instance.destroy();
  vk_command_buffers.clear();
  swapchain.reset();

  initialized = false;
}

void VulkanEngine::VulkanManager::cleanupCommandBuffers() {
  vk_device.freeCommandBuffers(vk_command_pool, vk_command_buffers);
  vk_device.destroyCommandPool(vk_command_pool);
  vk_command_buffers.clear();
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
