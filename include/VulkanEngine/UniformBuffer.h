// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_UNIFORMBUFFER_H_
#define INCLUDE_VULKANENGINE_UNIFORMBUFFER_H_

#include <VulkanEngine/Buffer.h>
#include <VulkanEngine/Descriptor.h>

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Represents a uniform buffer.
/// \tparam The data type this UniformBuffer represents.
template <typename T>
class UniformBuffer : public Buffer, public Descriptor {
 public:
  /// Constructor.
  /// \param _binding The binding index.
  /// \param _array_size For arrays, specify the array size.
  /// \param _vk_shader_stage_flags Specify which shader stages will access the
  /// buffer.
  UniformBuffer(uint32_t _binding, uint32_t _array_size = 1,
                vk::ShaderStageFlags _vk_shader_stage_flags =
                    vk::ShaderStageFlagBits::eAllGraphics);

  /// Destructor.
  virtual ~UniformBuffer();

  virtual void appendVkDescriptorSets(
      std::shared_ptr<std::vector<vk::WriteDescriptorSet>>
          write_descriptor_sets,
      std::shared_ptr<std::vector<vk::CopyDescriptorSet>> copy_descriptor_sets,
      const vk::DescriptorSet &destination_set);

 private:
  /// For arrays, specifies the array size.
  uint32_t array_size;

  std::vector<vk::DescriptorBufferInfo> vk_descriptor_buffer_infos;
};

}  // namespace VulkanEngine

#include <UniformBuffer.cpp>  // NOLINT(build/include)

#endif  // INCLUDE_VULKANENGINE_UNIFORMBUFFER_H_
