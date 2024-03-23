#include <VulkanEngine/Image.h>
#include <VulkanEngine/RenderPass.h>

#include "VulkanEngine/VulkanManager.h"

VulkanEngine::RenderPass::RenderPass(uint32_t width, uint32_t height) {
  depth_stencil_attachment.reset(new DepthStencilImageAttachment(
      vk::ImageLayout::eUndefined,
      vk::ImageUsageFlagBits::eDepthStencilAttachment,
      VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, width, height, 1, 4, false));

  depth_stencil_attachment->createImageView(vk::ImageViewType::e2D,
                                            vk::ImageAspectFlagBits::eDepth);
  depth_stencil_attachment->transitionImageLayout(
      vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto depth_attachment_description =
      vk::AttachmentDescription()
          .setFormat(depth_stencil_attachment->getVkFormat())
          .setSamples(depth_stencil_attachment->getVkSampleCountFlags())
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

  auto depth_attachment_reference =
      vk::AttachmentReference().setAttachment(1).setLayout(
          vk::ImageLayout::eDepthStencilAttachmentOptimal);

  color_attachment.reset(new ColorAttachment(
      vk::ImageLayout::eUndefined, vk::ImageUsageFlagBits::eColorAttachment,
      VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY, width, height, 1, 4, false));

  color_attachment->createImageView(vk::ImageViewType::e2D,
                                    vk::ImageAspectFlagBits::eColor);
  color_attachment->transitionImageLayout(
      vk::ImageLayout::eColorAttachmentOptimal);

  auto color_attachment_description =
      vk::AttachmentDescription()
          .setFormat(color_attachment->getVkFormat())
          .setSamples(color_attachment->getVkSampleCountFlags())
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

  auto color_attachment_reference =
      vk::AttachmentReference().setAttachment(0).setLayout(
          vk::ImageLayout::eColorAttachmentOptimal);

  auto color_attachment_resolve_description =
      vk::AttachmentDescription()
          .setFormat(vk::Format::eB8G8R8A8Unorm)
          .setSamples(vk::SampleCountFlagBits::e1)
          .setLoadOp(vk::AttachmentLoadOp::eClear)
          .setStoreOp(vk::AttachmentStoreOp::eStore)
          .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
          .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
          .setInitialLayout(vk::ImageLayout::eUndefined)
          .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

  auto color_attachment_resolve_reference =
      vk::AttachmentReference().setAttachment(2).setLayout(
          vk::ImageLayout::eColorAttachmentOptimal);

  auto subpass_description =
      vk::SubpassDescription()
          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
          .setColorAttachmentCount(1)
          .setPColorAttachments(&color_attachment_reference)
          .setPDepthStencilAttachment(&depth_attachment_reference)
          .setPResolveAttachments(&color_attachment_resolve_reference);

  auto dependency =
      vk::SubpassDependency()
          .setSrcSubpass(VK_SUBPASS_EXTERNAL)
          .setDstSubpass(0)
          .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
          .setSrcAccessMask(vk::AccessFlags())
          .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead |
                            vk::AccessFlagBits::eColorAttachmentWrite);

  std::array<vk::AttachmentDescription, 3> attachment_descriptions = {
      color_attachment_description, depth_attachment_description,
      color_attachment_resolve_description};

  auto render_pass_info = vk::RenderPassCreateInfo()
                              .setAttachmentCount(static_cast<uint32_t>(
                                  attachment_descriptions.size()))
                              .setPAttachments(attachment_descriptions.data())
                              .setSubpassCount(1)
                              .setPSubpasses(&subpass_description)
                              .setDependencyCount(1)
                              .setPDependencies(&dependency);

  vk_render_pass = VulkanManager::getInstance()->getVkDevice().createRenderPass(
      render_pass_info);
}

VulkanEngine::RenderPass::~RenderPass() {
  VulkanManager::getInstance()->getVkDevice().destroyRenderPass(vk_render_pass);
  vk_render_pass = nullptr;
}

const vk::RenderPass &VulkanEngine::RenderPass::getVkRenderPass() const {
  return vk_render_pass;
}
