#include <VulkanEngine/GraphicsPipeline.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/Shader.h>
#include <VulkanEngine/VulkanManager.h>

VulkanEngine::GraphicsPipeline::GraphicsPipeline() {}

VulkanEngine::GraphicsPipeline::~GraphicsPipeline() {
  if (vk_graphics_pipeline) {
    VulkanManager::getInstance().getVkDevice().destroyPipeline(
        vk_graphics_pipeline);
  }
}

void VulkanEngine::GraphicsPipeline::bindPipeline() {
  auto& vulkan_manager = VulkanManager::getInstance();
  vk::CommandBuffer current_command_buffer =
      vulkan_manager.getCurrentCommandBuffer();
  current_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                      vk_graphics_pipeline);
}

void VulkanEngine::GraphicsPipeline::setViewPort(float x, float y, float width,
                                                 float height, float min_depth,
                                                 float max_depth) {
  vk_viewport = vk::Viewport()
                    .setX(x)
                    .setY(y)
                    .setWidth(width)
                    .setHeight(height)
                    .setMinDepth(min_depth)
                    .setMaxDepth(max_depth);
}

void VulkanEngine::GraphicsPipeline::setScissor(int32_t offset_x,
                                                int32_t offset_y, int32_t width,
                                                int32_t height) {
  vk_scissor = vk::Rect2D()
                   .setOffset(vk::Offset2D(offset_x, offset_y))
                   .setExtent(vk::Extent2D(width, height));
}

void VulkanEngine::GraphicsPipeline::createGraphicsPipeline(
    const std::shared_ptr<MeshBase> mesh,
    const std::shared_ptr<Shader> shader) {
  if (vk_graphics_pipeline) {
    VulkanManager::getInstance().getVkDevice().destroyPipeline(
        vk_graphics_pipeline);
  }

  auto viewport_info = vk::PipelineViewportStateCreateInfo()
                           .setPScissors(&vk_scissor)
                           .setScissorCount(1)
                           .setPViewports(&vk_viewport)
                           .setViewportCount(1);

  auto rasterization_info = vk::PipelineRasterizationStateCreateInfo()
                                .setRasterizerDiscardEnable(VK_FALSE)
                                .setLineWidth(1.0f)
                                .setPolygonMode(vk::PolygonMode::eFill)
                                .setCullMode(vk::CullModeFlagBits::eBack)
                                .setFrontFace(vk::FrontFace::eCounterClockwise)
                                .setDepthBiasEnable(VK_FALSE);

  auto multisampling_info =
      vk::PipelineMultisampleStateCreateInfo()
          .setSampleShadingEnable(VK_TRUE)
          .setRasterizationSamples(vk::SampleCountFlagBits::e4);

  auto colorblend_attachment_info =
      vk::PipelineColorBlendAttachmentState()
          .setColorWriteMask(
              vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
              vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
          .setBlendEnable(VK_FALSE)
          .setSrcColorBlendFactor(vk::BlendFactor::eOne)
          .setDstColorBlendFactor(vk::BlendFactor::eZero)
          .setColorBlendOp(vk::BlendOp::eAdd)
          .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
          .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
          .setAlphaBlendOp(vk::BlendOp::eAdd);

  auto colorblend_info = vk::PipelineColorBlendStateCreateInfo()
                             .setLogicOpEnable(VK_FALSE)
                             .setAttachmentCount(1)
                             .setPAttachments(&colorblend_attachment_info);

  auto depth_stencil_state_create_info =
      vk::PipelineDepthStencilStateCreateInfo()
          .setDepthTestEnable(VK_TRUE)
          .setDepthWriteEnable(VK_TRUE)
          .setDepthCompareOp(vk::CompareOp::eLess)
          .setDepthBoundsTestEnable(VK_FALSE)
          .setMinDepthBounds(0.0f)
          .setMaxDepthBounds(1.0f);

  auto graphics_pipeline_info =
      vk::GraphicsPipelineCreateInfo()
          .setStageCount(
              static_cast<uint32_t>(shader->getVkShaderStages().size()))
          .setPStages(shader->getVkShaderStages().data())
          .setPVertexInputState(
              &mesh->createVkPipelineVertexInputStateCreateInfo())
          .setPInputAssemblyState(
              &mesh->createVkPipelineInputAssemblyStateCreateInfo())
          .setPViewportState(&viewport_info)
          .setPRasterizationState(&rasterization_info)
          .setPMultisampleState(&multisampling_info)
          .setPDepthStencilState(&depth_stencil_state_create_info)
          .setPColorBlendState(&colorblend_info)
          .setPDynamicState(nullptr)
          .setLayout(shader->createVkPipelineLayout())
          .setRenderPass(VulkanManager::getInstance()
                             .getDefaultRenderPass()
                             ->getVkRenderPass())
          .setSubpass(0);

  vk_graphics_pipeline =
      VulkanManager::getInstance()
          .getVkDevice()
          .createGraphicsPipeline(nullptr, graphics_pipeline_info)
          .value;
}
