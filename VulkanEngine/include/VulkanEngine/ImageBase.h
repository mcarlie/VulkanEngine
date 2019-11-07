#ifndef IMAGEBASE_H
#define IMAGEBASE_H

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

  class ImageBase {
    
  public:
    
    ImageBase();
    
    ~ImageBase();
    
    const vk::Image& getVkImage();
    
    const vk::ImageView& getVkImageView();

  protected:

    vk::Image vk_image;
    
    vk::ImageView vk_image_view;

  };

}

#endif /* IMAGEBASE_H */
