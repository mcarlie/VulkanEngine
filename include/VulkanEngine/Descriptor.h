// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_DESCRIPTOR_H_
#define INCLUDE_VULKANENGINE_DESCRIPTOR_H_

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Base class for any classes which interface with the Vulkan resource
/// descriptor concept.
class Descriptor {
 public:
  /// Constructor.
  Descriptor(uint32_t _binding, uint32_t _desciptor_count,
             vk::DescriptorType _vk_descriptor_type,
             vk::ShaderStageFlags _vk_shader_stage_flags);

  /// Destructor.
  ~Descriptor();

  /// \return The vk::DescriptorSetLayoutBinding for this UniformBuffer.
  const vk::DescriptorSetLayoutBinding getVkDescriptorSetLayoutBinding() const;

  /// \return A vk::DescriptorPoolSize describing this descriptor for creating a
  /// descriptor pool.
  const vk::DescriptorPoolSize getVkDescriptorPoolSize() const;

  /// Appends any write descriptor sets or copy descriptor sets which belong
  /// to this Descriptor to the given vectors.
  /// \param [out] write_descriptor_sets Any vk::WriteDescriptorSets of the
  /// descriptor should be appended to this vector. \param [out]
  /// copy_descriptor_sets Any vk::CopyDescriptorSets of the descriptor should
  /// be appended to this vector. \param destination_set The destination
  /// descriptor set of the write and copy descriptor sets.
  virtual void appendVkDescriptorSets(
      std::shared_ptr<std::vector<vk::WriteDescriptorSet>>
          write_descriptor_sets,
      std::shared_ptr<std::vector<vk::CopyDescriptorSet>> copy_descriptor_sets,
      const vk::DescriptorSet& destination_set) = 0;

 protected:
  /// The binding index
  uint32_t binding;

  /// The number of descriptors represented
  uint32_t descriptor_count;

  /// The type of descriptor represented
  vk::DescriptorType vk_descriptor_type;

  // Specifies which shader stages will access the buffer
  vk::ShaderStageFlags vk_shader_stage_flags;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_DESCRIPTOR_H_
