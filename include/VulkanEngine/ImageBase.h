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
  virtual ~ImageBase();

  /// Get the interal vulkan image.
  virtual vk::Image getVkImage() const = 0;

  /// Get the internal vulkan image view.
  virtual vk::ImageView getVkImageView() const = 0;
};

} // namespace VulkanEngine

#endif /* IMAGEBASE_H */
