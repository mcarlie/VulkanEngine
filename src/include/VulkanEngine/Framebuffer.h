#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vulkan/vulkan.hpp>

#include <memory>

namespace VulkanEngine {

class RenderPass;
class ImageBase;

/// TODO: This is a work in progress and is not currently used.
class Framebuffer {
public:
  Framebuffer(const std::shared_ptr<RenderPass> &render_pass,
              const std::vector<std::shared_ptr<ImageBase>> &attachments,
              uint32_t width, uint32_t height, uint32_t layers = 1);

  ~Framebuffer();

private:
  vk::Framebuffer vk_framebuffer;
};

} // namespace VulkanEngine

#endif /* FRAMEBUFFER_H */
