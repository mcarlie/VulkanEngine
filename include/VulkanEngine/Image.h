// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_IMAGE_H_
#define INCLUDE_VULKANENGINE_IMAGE_H_

#include <VulkanEngine/ImageBase.h>
#include <VulkanEngine/StagedBufferDestination.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#endif
#include <vk_mem_alloc.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace VulkanEngine {

/// Represents an image.
/// \tparam format The vk::Format of the pixels in the image.
/// \tparam image_type The vk::ImageType specifying the dimensions of the image.
/// \tparam tiling The vk::ImageTiling option to use.
/// \tparam sample_count_flags vk::SampleCountFlagBits option to use for the
/// image.
template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits  // NOLINT(whitespace/indent_namespace)
              sample_count_flags>  // NOLINT(whitespace/indent_namespace)
class Image : public StagedBufferDestination, public ImageBase {
 public:
  /// Constructor.
  /// \param initial_layout The initial vk::ImageLayout of the Image.
  /// \param usage_flags vk::ImageUsageFlags specifying how the image will be
  /// used. \param vma_memory_usage VmaMemoryUsage flags to pass to the Vulkan
  /// memory allocator library when allocating the image. \param _width The
  /// width of the image. \param _height The height of the image. \param _depth
  /// The depth of the image. \param pixel_size The data size of a single pixel
  /// in the image.
  Image(vk::ImageLayout initial_layout, vk::ImageUsageFlags usage_flags,
        VmaMemoryUsage vma_memory_usage, uint32_t _width, uint32_t _height,
        uint32_t _depth, size_t pixel_size, bool generate_mip_maps);

  /// Destructor.
  virtual ~Image();

  /// Sets the image data from the given pointer.
  /// \param data Pointer to the image data.
  void setImageData(const void *data);

  /// Create vk::ImageView.
  /// \param image_view_type The type of image view.
  /// \param image_aspect_flags Image aspect flags.
  void createImageView(vk::ImageViewType image_view_type,
                       vk::ImageAspectFlags image_aspect_flags);

  /// Transition the image from one vk::ImageLayout to another given by \c new
  /// layout by adding a vk::ImageMemoryBarrier to the command_buffer. \param
  /// new_layout The new layout to transition to. \param command_buffer The
  /// command buffer to add the command to. One will be created and added to the
  /// graphics queue if not specified.
  void transitionImageLayout(vk::ImageLayout new_layout,
                             const vk::CommandBuffer &command_buffer = nullptr);

  /// Get the vk::Format of the image.
  const vk::Format getVkFormat() const;

  /// Get vk::SampleCountFlagBits.
  const vk::SampleCountFlagBits getVkSampleCountFlags() const;

  /// Get the interal vulkan image.
  virtual vk::Image getVkImage() const;

  /// Get the vk::ImageView.
  virtual vk::ImageView getVkImageView() const;

 protected:
  /// Overridden to handle tranferring data from a StagedBuffer to this Image.
  /// \param command_buffers The command buffer to insert the command into.
  /// \param source_buffer The source vk::Buffer in the StagedBuffer.
  virtual void insertTransferCommand(const vk::CommandBuffer &command_buffer,
                                     const vk::Buffer &source_buffer);

  /// \return The data size for the staging buffer.
  virtual size_t getStagingBufferSize() const;

 private:
  /// Creates the Image and allocates memory.
  /// \param usage_flags vk::ImageUsageFlags specifying how the image will be
  /// used. \param vma_usage VmaMemoryUsage flags to pass to the Vulkan memory
  /// allocator library when allocating the image.
  void createImage(vk::ImageUsageFlags usage_flags, VmaMemoryUsage vma_usage);

  /// Generate mipmap images for this image.
  /// \param command_buffer The command buffer to use for generating the
  /// mipmaps.
  void generateMipmaps(const vk::CommandBuffer &command_buffer);

 protected:
  /// The width of the Image.
  uint32_t width;

  /// The height of the Image.
  uint32_t height;

  /// The depth of the Image.
  uint32_t depth;

  /// The total size of Image's data.
  size_t data_size;

  /// The number of mipmap images to generate.
  uint32_t mipmap_levels;

  /// The current vk::ImageLayout. \see transitionImageLayout().
  vk::ImageLayout vk_image_layout;

  /// Internal vk_image.
  vk::Image vk_image;

  /// The vk::ImageView created in createImageView().
  vk::ImageView vk_image_view;

  /// VkImageCreateInfo.
  VkImageCreateInfo image_create_info;
};

}  // namespace VulkanEngine

#include <Image.cpp>  // NOLINT(build/include)

#endif  // INCLUDE_VULKANENGINE_IMAGE_H_
