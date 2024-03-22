#ifndef SHADERIMAGE_H
#define SHADERIMAGE_H

#include <VulkanEngine/Descriptor.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <vk_mem_alloc.h>
#pragma clang diagnostic pop
#endif

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

// Forward declaration
template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
class Image;

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
class ShaderImage
    : public Image<format, image_type, tiling, sample_count_flags>,
      public Descriptor {
public:
  /// Constructor.
  /// \param initial_layout The initial vk::ImageLayout of the image.
  /// \param usage_flags vk::ImageUsageFlags specifying how the image will be
  /// used. \param vma_memory_usage VmaMemoryUsage flags to pass to the Vulkan
  /// memory allocator library when allocating the image. \param _width The
  /// width of the image. \param _height The height of the image. \param _depth
  /// The depth of the image. \param pixel_size The data size of a single pixel
  /// in the image.
  ShaderImage(vk::ImageLayout initial_layout, vk::ImageUsageFlags usage_flags,
              VmaMemoryUsage vma_memory_usage, uint32_t width, uint32_t height,
              uint32_t depth, size_t pixel_size, uint32_t binding,
              uint32_t descriptor_count, vk::DescriptorType descriptor_type,
              vk::ShaderStageFlags shader_stage_flags);

  /// Destructor.
  ~ShaderImage();

  void createSampler();

  vk::DescriptorImageInfo getVkDescriptorImageInfo();

  virtual void appendVkDescriptorSets(
      std::vector<vk::WriteDescriptorSet> &write_descriptor_sets,
      std::vector<vk::CopyDescriptorSet> &copy_descriptor_sets,
      const vk::DescriptorSet &destination_set);

private:
  /// The vk::Sampler created in createSampler().
  vk::Sampler vk_sampler;

  vk::DescriptorImageInfo vk_descriptor_image_info;
};

} // namespace VulkanEngine

#include <ShaderImage.cpp>

#endif /* SHADERIMAGE_H */