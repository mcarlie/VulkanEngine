#include <VulkanEngine/Framebuffer.h>
#include <VulkanEngine/RenderPass.h>
#include <VulkanEngine/ImageBase.h>

VulkanEngine::Framebuffer::Framebuffer(
  const std::shared_ptr< RenderPass >& render_pass,
  const std::vector< std::shared_ptr< ImageBase > >& attachments,
  uint32_t width,
  uint32_t height,
  uint32_t layers ) {
  
  std::vector< vk::ImageView > attachment_data;
  for( auto& attachment : attachments ) {
    attachment_data.push_back( attachment->getVkImageView() );
  }
  
  auto framebuffer_info = vk::FramebufferCreateInfo()
    .setRenderPass( render_pass->getVkRenderPass() )
    .setAttachmentCount( static_cast< uint32_t >( attachment_data.size() ) )
    .setPAttachments( attachment_data.data() )
    .setWidth( width )
    .setHeight( height )
    .setLayers( layers );

  vk_framebuffer = VulkanManager::getInstance()->getVkDevice().createFramebuffer( framebuffer_info );
  
}

VulkanEngine::Framebuffer::~Framebuffer() {
  VulkanManager::getInstance()->getVkDevice().destroyFramebuffer( vk_framebuffer );
}
