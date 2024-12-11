// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_IMAGEBASE_H_
#define INCLUDE_VULKANENGINE_IMAGEBASE_H_

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

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_IMAGEBASE_H_
