#include <VulkanEngine/ImageBase.h>

VulkanEngine::ImageBase::ImageBase() {
  
}

VulkanEngine::ImageBase::~ImageBase() {
  
}

const vk::Image& VulkanEngine::ImageBase::getVkImage() {
  return vk_image;
}

const vk::ImageView& VulkanEngine::ImageBase::getVkImageView() {
  return vk_image_view;
}
