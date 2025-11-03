// Copyright (c) 2025 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <VulkanEngine/Swapchain.h>

#include <limits>
#include <memory>

VulkanEngine::Swapchain::Swapchain(uint32_t number_of_images,
                                   std::shared_ptr<Window> _window,
                                   std::shared_ptr<RenderPass> render_pass)
    : window(_window) {
  auto& vulkan_manager = VulkanManager::getInstance();

  auto swapchain_info =
      vk::SwapchainCreateInfoKHR()
          .setSurface(window->getVkSurface())
          .setMinImageCount(number_of_images)
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

  auto vk_device = vulkan_manager.getDevice()->getVkDevice();

  vk_swapchain = vk_device.createSwapchainKHR(swapchain_info);
  vk_swapchain_images = vk_device.getSwapchainImagesKHR(vk_swapchain);

  vk_swapchain_image_views.resize(vk_swapchain_images.size());

  for (size_t i = 0; i < vk_swapchain_image_views.size(); ++i) {
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

    vk_swapchain_image_views[i] = vk_device.createImageView(image_view_info);
  }

  vk_swapchain_framebuffers.resize(vk_swapchain_image_views.size());
  for (size_t i = 0; i < vk_swapchain_framebuffers.size(); ++i) {
    std::array<vk::ImageView, 3> attachments = {
        render_pass->getColorAttachment()->getVkImageView(),
        render_pass->getDepthStencilAttachment()->getVkImageView(),
        vk_swapchain_image_views[i]};

    auto framebuffer_info =
        vk::FramebufferCreateInfo()
            .setRenderPass(render_pass->getVkRenderPass())
            .setAttachmentCount(static_cast<uint32_t>(attachments.size()))
            .setPAttachments(attachments.data())
            .setWidth(window->getFramebufferWidth())
            .setHeight(window->getFramebufferHeight())
            .setLayers(1);

    vk_swapchain_framebuffers[i] =
        vk_device.createFramebuffer(framebuffer_info);
  }

  auto semaphore_info = vk::SemaphoreCreateInfo();
  auto fence_info =
      vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);
  for (size_t i = 0; i < number_of_images; ++i) {
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

VulkanEngine::Swapchain::~Swapchain() {
  auto vk_device = VulkanManager::getInstance().getDevice()->getVkDevice();
  for (size_t i = 0; i < vk_swapchain_framebuffers.size(); ++i) {
    vk_device.destroyFramebuffer(vk_swapchain_framebuffers[i]);
  }
  vk_swapchain_framebuffers.clear();

  for (size_t i = 0; i < vk_swapchain_image_views.size(); ++i) {
    vk_device.destroyImageView(vk_swapchain_image_views[i]);
  }
  vk_swapchain_image_views.clear();

  vk_device.destroySwapchainKHR(vk_swapchain);
  vk_swapchain = nullptr;

  for (size_t i = 0; i < vk_image_available_semaphores.size(); ++i) {
    vk_device.destroySemaphore(vk_image_available_semaphores[i]);
    vk_device.destroySemaphore(vk_rendering_finished_semaphores[i]);
    vk_device.destroyFence(vk_in_flight_fences[i]);
  }
}

bool VulkanEngine::Swapchain::present() {
  uint32_t image_index;
  auto& vulkan_manager = VulkanManager::getInstance();
  auto vk_device = vulkan_manager.getDevice()->getVkDevice();

  const uint64_t current_frame = vulkan_manager.getCurrentFrame();

  // Acquire the next available swapchain image that we can write to
  vk::Result result = vk_device.acquireNextImageKHR(
      vk_swapchain, std::numeric_limits<uint32_t>::max(),
      vk_image_available_semaphores[current_frame], nullptr, &image_index);

  // If the window size has changed or the image view is out of date
  // according to Vulkan then recreate the pipeline from the swapchain stage
  if (result == vk::Result::eErrorOutOfDateKHR || window->sizeHasChanged()) {
    return false;
  } else if (result != vk::Result::eSuccess &&
             result != vk::Result::eSuboptimalKHR) {
    throw std::runtime_error("Failed to acquire image!");
  }

  vk::Semaphore signal_semaphores[] = {
      vk_rendering_finished_semaphores[current_frame]};
  uint32_t signal_semaphores_count = 1;

  vk::Semaphore wait_semaphores[] = {
      vk_image_available_semaphores[current_frame]};
  vk::PipelineStageFlags wait_stages[] = {
      vk::PipelineStageFlagBits::eColorAttachmentOutput};

  auto command_buffer = vulkan_manager.getCurrentCommandBuffer();
  auto submit_info = vk::SubmitInfo()
                         .setWaitSemaphoreCount(1)
                         .setPWaitSemaphores(wait_semaphores)
                         .setPWaitDstStageMask(wait_stages)
                         .setCommandBufferCount(1)
                         .setPCommandBuffers(&command_buffer)
                         .setSignalSemaphoreCount(signal_semaphores_count)
                         .setPSignalSemaphores(signal_semaphores);

  auto fence_wait_result =
      vk_device.waitForFences(1, &vk_in_flight_fences[current_frame], VK_TRUE,
                              std::numeric_limits<uint64_t>::max());

  if (fence_wait_result != vk::Result::eSuccess) {
    if (fence_wait_result == vk::Result::eTimeout) {
      throw std::runtime_error("Timeout while waiting for fence!");
    } else {
      throw std::runtime_error("Failed to wait for fence!");
    }
  }

  vk_device.resetFences(vk_in_flight_fences[current_frame]);

  auto vk_graphics_queue = vulkan_manager.getDevice()->getVkGraphicsQueue();
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
  return true;
}

vk::Framebuffer VulkanEngine::Swapchain::getFramebuffer(size_t index) {
  return vk_swapchain_framebuffers[index];
}

vk::SwapchainKHR VulkanEngine::Swapchain::getVkSwapChain() {
  return vk_swapchain;
}

void VulkanEngine::Swapchain::waitForFence() {
  auto& vulkan_manager = VulkanManager::getInstance();
  auto fence_result = vulkan_manager.getDevice()->getVkDevice().waitForFences(
      vk_in_flight_fences[vulkan_manager.getCurrentFrame()], VK_TRUE,
      std::numeric_limits<uint32_t>::max());
  if (fence_result != vk::Result::eSuccess) {
    throw std::runtime_error("Error waiting for fences");
  }
}
