// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_FRAMEBUFFER_H_
#define INCLUDE_VULKANENGINE_FRAMEBUFFER_H_

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

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

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_FRAMEBUFFER_H_
