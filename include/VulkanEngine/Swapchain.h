#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/Window.h>

namespace VulkanEngine {

/// Class which represents the swapchain.
class Swapchain {
public:
  /// Constructor.
  Swapchain(uint32_t number_of_images, std::shared_ptr<Window> _window, std::shared_ptr<RenderPass> render_pass);

  /// Destructor.
  ~Swapchain();

  bool present();

  vk::Framebuffer getFramebuffer(size_t index);

  vk::SwapchainKHR getVkSwapChain();

  void waitForFence();

private:

  vk::SwapchainKHR vk_swapchain;
  std::vector<vk::Image> vk_swapchain_images;
  std::vector<vk::ImageView> vk_swapchain_image_views;
  std::vector<vk::Framebuffer> vk_swapchain_framebuffers;

  std::vector<vk::Semaphore> vk_image_available_semaphores;
  std::vector<vk::Semaphore> vk_rendering_finished_semaphores;
  std::vector<vk::Fence> vk_in_flight_fences;

  std::shared_ptr<Window> window;

};

} // namespace VulkanEngine

#endif /* SWAPCHAIN_H */
