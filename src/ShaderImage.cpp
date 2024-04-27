#ifndef SHADERIMAGE_CPP
#define SHADERIMAGE_CPP

#include <VulkanEngine/Image.h>
#include <VulkanEngine/ShaderImage.h>

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
VulkanEngine::ShaderImage<format, image_type, tiling, sample_count_flags>::
    ShaderImage(vk::ImageLayout initial_layout, vk::ImageUsageFlags usage_flags,
                VmaMemoryUsage vma_memory_usage, uint32_t width,
                uint32_t height, uint32_t depth, size_t pixel_size,
                uint32_t binding, uint32_t decriptor_count,
                vk::DescriptorType descriptor_type,
                vk::ShaderStageFlags shader_stage_flags)
    : Image<format, image_type, tiling, sample_count_flags>(
          initial_layout, usage_flags, vma_memory_usage, width, height, depth,
          pixel_size),
      Descriptor(binding, decriptor_count, descriptor_type,
                 shader_stage_flags) {}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
VulkanEngine::ShaderImage<format, image_type, tiling,
                          sample_count_flags>::~ShaderImage() {
  VulkanManager::getInstance().getVkDevice().destroySampler(vk_sampler);
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::ShaderImage<format, image_type, tiling,
                               sample_count_flags>::createSampler() {
  auto sampler_create_info =
      vk::SamplerCreateInfo()
          .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
          .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
          .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
          .setAnisotropyEnable(VK_TRUE)
          .setMaxAnisotropy(16.0f)
          .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
          .setMagFilter(vk::Filter::eLinear)
          .setMinFilter(vk::Filter::eLinear)
          .setUnnormalizedCoordinates(VK_FALSE)
          .setCompareEnable(VK_FALSE)
          .setCompareOp(vk::CompareOp::eAlways)
          .setMipmapMode(vk::SamplerMipmapMode::eLinear)
          .setMipLodBias(0.0f)
          .setMinLod(0.0f)
          .setMaxLod(static_cast<float>(this->mipmap_levels));

  vk_sampler = VulkanManager::getInstance().getVkDevice().createSampler(
      sampler_create_info);
  if (!vk_sampler) {
    throw std::runtime_error("Could not create sampler for image");
  }
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
vk::DescriptorImageInfo
VulkanEngine::ShaderImage<format, image_type, tiling,
                          sample_count_flags>::getVkDescriptorImageInfo()
    const {
  return vk::DescriptorImageInfo()
      .setSampler(vk_sampler)
      .setImageView(this->vk_image_view)
      .setImageLayout(this->vk_image_layout);
}

template <vk::Format format, vk::ImageType image_type, vk::ImageTiling tiling,
          vk::SampleCountFlagBits sample_count_flags>
void VulkanEngine::ShaderImage<format, image_type, tiling, sample_count_flags>::
    appendVkDescriptorSets(
        std::vector<vk::WriteDescriptorSet> &write_descriptor_sets,
        std::vector<vk::CopyDescriptorSet> &copy_descriptor_sets,
        const vk::DescriptorSet &destination_set) {
  vk_descriptor_image_info = vk::DescriptorImageInfo()
                                 .setSampler(vk_sampler)
                                 .setImageView(this->vk_image_view)
                                 .setImageLayout(this->vk_image_layout);

  write_descriptor_sets.push_back(
      vk::WriteDescriptorSet()
          .setDstBinding(binding)
          .setDstArrayElement(0)
          .setDstSet(destination_set)
          .setDescriptorType(vk_descriptor_type)
          .setDescriptorCount(1)
          .setPImageInfo(&vk_descriptor_image_info));
}

#endif /* SHADERIMAGE_CPP */
