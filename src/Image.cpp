#ifndef IMAGE_CPP
#define IMAGE_CPP

#include <VulkanEngine/Image.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Device.h>

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
VulkanEngine::Image<format, image_type, tiling, sample_count_flags>::Image(
    vk::ImageLayout initial_layout, vk::ImageUsageFlags usage_flags,
    VmaMemoryUsage vma_memory_usage, uint32_t _width, uint32_t _height,
    uint32_t _depth, size_t pixel_size, bool generate_mip_maps)
    : StagedBufferDestination(), ImageBase(), width(_width), height(_height),
      depth(_depth), data_size(pixel_size * width * height * depth),
      mipmap_levels(1), vk_image_layout(initial_layout) {
  if (generate_mip_maps) {
    mipmap_levels =
        static_cast<uint32_t>(std::floor(std::log2(std::max(width, height))));
  }
  createImage(usage_flags, vma_memory_usage);
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
VulkanEngine::Image<format, image_type, tiling, sample_count_flags>::~Image() {
  VulkanManager::getInstance().getDevice()->getVkDevice().destroyImageView(vk_image_view);
  vmaDestroyImage(VulkanManager::getInstance().getDevice()->getVmaAllocator(), vk_image,
                  vma_allocation);
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::Image<format, image_type, tiling,
                         sample_count_flags>::setImageData(const void *data) {
  updateBuffer(data, data_size);
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::Image<format, image_type, tiling, sample_count_flags>::
    createImageView(vk::ImageViewType image_view_type,
                    vk::ImageAspectFlags image_aspect_flags) {
  auto subresource_range = vk::ImageSubresourceRange()
                               .setAspectMask(image_aspect_flags)
                               .setBaseMipLevel(0)
                               .setLevelCount(mipmap_levels)
                               .setBaseArrayLayer(0)
                               .setLayerCount(1);

  auto image_view_create_info = vk::ImageViewCreateInfo()
                                    .setFormat(format)
                                    .setImage(vk_image)
                                    .setViewType(image_view_type)
                                    .setSubresourceRange(subresource_range);

  vk_image_view = VulkanManager::getInstance().getDevice()->getVkDevice().createImageView(
      image_view_create_info);
  if (!vk_image_view) {
    throw std::runtime_error("Could not create image view for image!");
  }
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::Image<format, image_type, tiling, sample_count_flags>::
    transitionImageLayout(vk::ImageLayout new_layout,
                          const vk::CommandBuffer &command_buffer) {
  const vk::CommandBuffer &command_buffer_to_use =
      command_buffer ? command_buffer : single_use_command_buffer;
  bool created_single_use_command_buffer = false;
  if (!command_buffer_to_use) {
    created_single_use_command_buffer = true;
    beginSingleUsageCommandBuffer();
  }

  auto subresource_range = vk::ImageSubresourceRange()
                               .setAspectMask(vk::ImageAspectFlagBits::eColor)
                               .setBaseMipLevel(0)
                               .setLevelCount(mipmap_levels)
                               .setBaseArrayLayer(0)
                               .setLayerCount(1);

  vk::PipelineStageFlagBits source_stage;
  vk::PipelineStageFlagBits destination_stage;

  auto image_memory_barrier =
      vk::ImageMemoryBarrier()
          .setImage(vk_image)
          .setOldLayout(vk_image_layout)
          .setNewLayout(new_layout)
          .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
          .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

  if (vk_image_layout == vk::ImageLayout::eUndefined &&
      new_layout == vk::ImageLayout::eTransferDstOptimal) {
    image_memory_barrier.setSrcAccessMask(vk::AccessFlags());
    image_memory_barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

    source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
    destination_stage = vk::PipelineStageFlagBits::eTransfer;

  } else if (vk_image_layout == vk::ImageLayout::eUndefined &&
             new_layout == vk::ImageLayout::eColorAttachmentOptimal) {
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);

    image_memory_barrier.setSrcAccessMask(vk::AccessFlags());
    image_memory_barrier.setDstAccessMask(
        vk::AccessFlagBits::eColorAttachmentRead |
        vk::AccessFlagBits::eColorAttachmentWrite);

    source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
    destination_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

  } else if (vk_image_layout == vk::ImageLayout::eUndefined &&
             new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
    subresource_range.setAspectMask(vk::ImageAspectFlagBits::eDepth |
                                    vk::ImageAspectFlagBits::eStencil);

    image_memory_barrier.setSrcAccessMask(vk::AccessFlags());
    image_memory_barrier.setDstAccessMask(
        vk::AccessFlagBits::eDepthStencilAttachmentRead |
        vk::AccessFlagBits::eDepthStencilAttachmentWrite);

    source_stage = vk::PipelineStageFlagBits::eTopOfPipe;
    destination_stage = vk::PipelineStageFlagBits::eEarlyFragmentTests;

  } else if (vk_image_layout == vk::ImageLayout::eTransferDstOptimal &&
             new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
    image_memory_barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
    image_memory_barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

    source_stage = vk::PipelineStageFlagBits::eTransfer;
    destination_stage = vk::PipelineStageFlagBits::eFragmentShader;

  } else {
    throw std::runtime_error("Invalid image transfer request!");
  }

  image_memory_barrier.setSubresourceRange(subresource_range);
  command_buffer_to_use.pipelineBarrier(source_stage, destination_stage,
                                        vk::DependencyFlags(), 0, 0,
                                        image_memory_barrier);

  vk_image_layout = new_layout;

  if (created_single_use_command_buffer) {
    endSingleUsageCommandBuffer();
  }
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
const vk::Format VulkanEngine::Image<format, image_type, tiling,
                                     sample_count_flags>::getVkFormat() const {
  return format;
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
const vk::SampleCountFlagBits
VulkanEngine::Image<format, image_type, tiling,
                    sample_count_flags>::getVkSampleCountFlags() const {
  return sample_count_flags;
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
vk::Image VulkanEngine::Image<format, image_type, tiling,
                              sample_count_flags>::getVkImage() const {
  return vk_image;
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
vk::ImageView VulkanEngine::Image<format, image_type, tiling,
                                  sample_count_flags>::getVkImageView() const {
  return vk_image_view;
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::Image<format, image_type, tiling, sample_count_flags>::
    insertTransferCommand(const vk::CommandBuffer &command_buffer,
                          const vk::Buffer &source_buffer) {
  transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, command_buffer);

  auto image_subresource = vk::ImageSubresourceLayers()
                               .setAspectMask(vk::ImageAspectFlagBits::eColor)
                               .setMipLevel(0)
                               .setBaseArrayLayer(0)
                               .setLayerCount(1);

  auto image_offset = vk::Offset3D().setX(0).setY(0).setZ(0);

  auto image_extent =
      vk::Extent3D().setWidth(width).setHeight(height).setDepth(depth);

  auto buffer_image_copy = vk::BufferImageCopy()
                               .setBufferOffset(0)
                               .setBufferRowLength(0)
                               .setBufferImageHeight(0)
                               .setImageSubresource(image_subresource)
                               .setImageOffset(image_offset)
                               .setImageExtent(image_extent);

  command_buffer.copyBufferToImage(source_buffer, vk_image,
                                   vk::ImageLayout::eTransferDstOptimal,
                                   buffer_image_copy);

  if (mipmap_levels > 1) {
    generateMipmaps(command_buffer);
  } else {
    transitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal,
                          command_buffer);
  }
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
size_t VulkanEngine::Image<format, image_type, tiling,
                           sample_count_flags>::getStagingBufferSize() const {
  return data_size;
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::Image<format, image_type, tiling, sample_count_flags>::
    createImage(vk::ImageUsageFlags usage_flags, VmaMemoryUsage vma_usage) {
  auto image_extent =
      vk::Extent3D().setWidth(width).setHeight(height).setDepth(depth);

  image_create_info = static_cast<VkImageCreateInfo>(
      vk::ImageCreateInfo()
          .setImageType(image_type)
          .setExtent(image_extent)
          .setMipLevels(mipmap_levels)
          .setArrayLayers(1)
          .setFormat(format)
          .setTiling(tiling)
          .setInitialLayout(vk_image_layout)
          .setUsage(usage_flags)
          .setSharingMode(vk::SharingMode::eExclusive)
          .setSamples(sample_count_flags));

  VmaAllocationCreateInfo allocate_info = {};
  allocate_info.usage = vma_usage;

  VkImage c_image_handle;
  auto allocation_result = vmaCreateImage(
      VulkanManager::getInstance().getDevice()->getVmaAllocator(), &image_create_info,
      &allocate_info, &c_image_handle, &vma_allocation, nullptr);
  if (allocation_result != VK_SUCCESS) {
    throw std::runtime_error("Could not allocate Image memory!");
  }

  vk_image = c_image_handle;
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::Image<format, image_type, tiling, sample_count_flags>::
    generateMipmaps(const vk::CommandBuffer &command_buffer) {

  auto subresource_range = vk::ImageSubresourceRange()
                               .setAspectMask(vk::ImageAspectFlagBits::eColor)
                               .setLevelCount(1)
                               .setBaseArrayLayer(0)
                               .setLayerCount(1);

  auto image_memory_barrier =
      vk::ImageMemoryBarrier()
          .setImage(vk_image)
          .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
          .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

  uint32_t mip_width = width;
  uint32_t mip_height = height;

  // Blit the base image to the first mip map level.
  // Then do the same for each level after that.
  for (uint32_t i = 1; i < mipmap_levels; ++i) {
    subresource_range.setBaseMipLevel(i - 1);

    image_memory_barrier.setSubresourceRange(subresource_range)
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eTransferRead);

    command_buffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
        vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

    auto blit_src_subresource =
        vk::ImageSubresourceLayers()
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setMipLevel(i - 1)
            .setBaseArrayLayer(0)
            .setLayerCount(1);

    auto blit_dst_subresource =
        vk::ImageSubresourceLayers()
            .setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setMipLevel(i)
            .setBaseArrayLayer(0)
            .setLayerCount(1);

    std::array<vk::Offset3D, 2> blit_src_offsets = {
        vk::Offset3D(0, 0, 0), vk::Offset3D(mip_width, mip_height, 1)};
    std::array<vk::Offset3D, 2> blit_dst_offsets = {
        vk::Offset3D(0, 0, 0),
        vk::Offset3D(mip_width > 1 ? mip_width / 2 : 1,
                     mip_height > 1 ? mip_height / 2 : 1, 1)};
    auto image_blit =
        vk::ImageBlit()
            .setSrcOffsets(blit_src_offsets)
            .setSrcSubresource(blit_src_subresource)
            .setDstOffsets(blit_dst_offsets)
            .setDstSubresource(blit_dst_subresource);

    command_buffer.blitImage(
        vk_image, vk::ImageLayout::eTransferSrcOptimal, vk_image,
        vk::ImageLayout::eTransferDstOptimal, 1, &image_blit,
        vk::Filter::eLinear);

    // Scale next mip map level.
    if (mip_width > 1) mip_width /= 2;
    if (mip_height > 1) mip_height /= 2;
  }

  // Transition all mip levels to eShaderReadOnlyOptimal
  subresource_range.setBaseMipLevel(0);
  subresource_range.setLevelCount(mipmap_levels - 1);
  image_memory_barrier.setSubresourceRange(subresource_range)
      .setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
      .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
      .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
      .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

  command_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
      vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

  // The last mip map level was never set to eTransferSrcOptimal
  subresource_range.setBaseMipLevel(mipmap_levels - 1);
  subresource_range.setLevelCount(1);
  image_memory_barrier.setSubresourceRange(subresource_range)
      .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
      .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
      .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
      .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

  command_buffer.pipelineBarrier(
      vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
      vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

  vk_image_layout = vk::ImageLayout::eShaderReadOnlyOptimal;
}

#endif /* IMAGE_CPP */
