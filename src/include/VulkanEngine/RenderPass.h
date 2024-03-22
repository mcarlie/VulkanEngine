#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <VulkanEngine/VulkanManager.h>

#include <cstdint>
#include <memory>
#include <vector>

// Forward decleration.
class Image;

/// TODO development of this class is in progress.
namespace VulkanEngine {
class RenderPass {
public:
  using DepthStencilImageAttachment =
      Image<vk::Format::eD32SfloatS8Uint, vk::ImageType::e2D,
            vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e4>;

  using ColorAttachment =
      Image<vk::Format::eB8G8R8A8Unorm, vk::ImageType::e2D,
            vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e4>;

public:
  RenderPass(uint32_t width, uint32_t height);

  ~RenderPass();

  const vk::RenderPass &getVkRenderPass();

  const std::shared_ptr<DepthStencilImageAttachment>
  getDepthStencilAttachment() {
    return depth_stencil_attachment;
  }

  const std::shared_ptr<ColorAttachment> getColorAttachment() {
    return color_attachment;
  }

private:
  std::shared_ptr<DepthStencilImageAttachment> depth_stencil_attachment;

  std::shared_ptr<ColorAttachment> color_attachment;

  vk::RenderPass vk_render_pass;
};

} // namespace VulkanEngine

#endif /* RENDERPASS_H */
