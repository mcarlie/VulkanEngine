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

#include <VulkanEngine/Framebuffer.h>
#include <VulkanEngine/ImageBase.h>
#include <VulkanEngine/RenderPass.h>

#include <memory>
#include <vector>

VulkanEngine::Framebuffer::Framebuffer(
    const std::shared_ptr<RenderPass>& render_pass,
    const std::vector<std::shared_ptr<ImageBase>>& attachments, uint32_t width,
    uint32_t height, uint32_t layers) {
  std::vector<vk::ImageView> attachment_data;
  for (auto& attachment : attachments) {
    attachment_data.push_back(attachment->getVkImageView());
  }

  auto framebuffer_info =
      vk::FramebufferCreateInfo()
          .setRenderPass(render_pass->getVkRenderPass())
          .setAttachmentCount(static_cast<uint32_t>(attachment_data.size()))
          .setPAttachments(attachment_data.data())
          .setWidth(width)
          .setHeight(height)
          .setLayers(layers);

  vk_framebuffer =
      VulkanManager::getInstance().getDevice()->getVkDevice().createFramebuffer(
          framebuffer_info);
}

VulkanEngine::Framebuffer::~Framebuffer() {
  VulkanManager::getInstance().getDevice()->getVkDevice().destroyFramebuffer(
      vk_framebuffer);
}
