#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include <VulkanEngine/Buffer.h>
#include <VulkanEngine/Descriptor.h>

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

  /// Represents a uniform buffer.
  /// \tparam The data type this UniformBuffer represents.
  template< typename T >
  class UniformBuffer : public Buffer, public Descriptor {

  public:

    /// Constructor.
    /// \param _binding The binding index.
    /// \param _array_size For arrays, specify the array size.
    /// \param _vk_shader_stage_flags Specify which shader stages will access the buffer.
    UniformBuffer( 
      uint32_t _binding,
      uint32_t _array_size = 1,
      vk::ShaderStageFlags _vk_shader_stage_flags = vk::ShaderStageFlagBits::eAllGraphics );
    
    /// Destructor.
    ~UniformBuffer();

    virtual void appendVkDescriptorSets( 
      std::vector< vk::WriteDescriptorSet >& write_descriptor_sets,
      std::vector< vk::CopyDescriptorSet >& copy_descriptor_sets,
      const vk::DescriptorSet& destination_set );

  private:

    /// For arrays, specifies the array size.
    uint32_t array_size;

    std::vector< vk::DescriptorBufferInfo > vk_descriptor_buffer_infos;

  };

}

#include <UniformBuffer.cpp>

#endif /* UNIFORMBUFFER_H */