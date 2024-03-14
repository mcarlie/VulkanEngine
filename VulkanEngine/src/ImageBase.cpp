#include <VulkanEngine/ImageBase.h>

VulkanEngine::ImageBase::ImageBase() {}

VulkanEngine::ImageBase::~ImageBase() {}

const vk::Image VulkanEngine::ImageBase::getVkImage() const { return vk_image; }

const vk::ImageView VulkanEngine::ImageBase::getVkImageView() const {
  return vk_image_view;
}
