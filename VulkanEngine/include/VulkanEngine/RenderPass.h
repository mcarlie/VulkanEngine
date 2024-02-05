#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <VulkanEngine/Image.h>
#include <VulkanEngine/VulkanManager.h>


namespace VulkanEngine {

class RenderPass {

public:
  RenderPass();

  ~RenderPass();

  void createRenderPass(const std::shared_ptr<Window> &window);

  const vk::RenderPass &getVkRenderPass();

private:
  using DepthStencilImageAttachment =
      Image<vk::Format::eD32SfloatS8Uint, vk::ImageType::e2D,
            vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e4>;

  using ColorAttachment =
      Image<vk::Format::eB8G8R8A8Unorm, vk::ImageType::e2D,
            vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e4>;

  std::shared_ptr<DepthStencilImageAttachment> depth_stencil_attachment;

  std::shared_ptr<ColorAttachment> color_attachment;

  vk::RenderPass vk_render_pass;
};

} // namespace VulkanEngine

#endif /* RENDERPASS_H */
