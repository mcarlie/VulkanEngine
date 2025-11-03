// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_SHADERIMAGE_H_
#define INCLUDE_VULKANENGINE_SHADERIMAGE_H_

#include <VulkanEngine/Descriptor.h>

#include <memory>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#endif
#include <vk_mem_alloc.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

// Forward declaration
template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits  // NOLINT(whitespace/indent_namespace)
              sample_count_flags>  // NOLINT(whitespace/indent_namespace)
class Image;

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits  // NOLINT(whitespace/indent_namespace)
              sample_count_flags>  // NOLINT(whitespace/indent_namespace)
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

  /// Create image sampler.
  void createSampler();

  /// Get descriptor image info.
  vk::DescriptorImageInfo getVkDescriptorImageInfo() const;

  /// Append descriptor sets to this shader image.
  virtual void appendVkDescriptorSets(
      std::shared_ptr<std::vector<vk::WriteDescriptorSet>>
          write_descriptor_sets,
      std::shared_ptr<std::vector<vk::CopyDescriptorSet>> copy_descriptor_sets,
      const vk::DescriptorSet& destination_set);

 private:
  /// The vk::Sampler created in createSampler().
  vk::Sampler vk_sampler;

  vk::DescriptorImageInfo vk_descriptor_image_info;
};

}  // namespace VulkanEngine

#include <ShaderImage.cpp>  // NOLINT(build/include)

#endif  // INCLUDE_VULKANENGINE_SHADERIMAGE_H_
