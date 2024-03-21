#ifndef IMAGEBASE_H
#define IMAGEBASE_H

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Base class for image types.
class ImageBase {
public:
  /// Contructor.
  ImageBase();

  /// Destructor.
  ~ImageBase();

  /// Get the interal vulkan image.
  const vk::Image getVkImage() const;

  /// Get the internal vulkan image view.
  const vk::ImageView getVkImageView() const;

protected:
  /// The internal vulkan image.
  vk::Image vk_image;

  /// The internal vulkan image view.
  vk::ImageView vk_image_view;
};

} // namespace VulkanEngine

#endif /* IMAGEBASE_H */
