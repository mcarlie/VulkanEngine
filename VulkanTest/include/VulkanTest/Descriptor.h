#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  /// Base class for any classes interface with the Vulkan resource descriptor concept
  class Descriptor {

  public:

    /// Constructor
    Descriptor( uint32_t _binding, uint32_t _desciptor_count, vk::DescriptorType _vk_descriptor_type, vk::ShaderStageFlags _vk_shader_stage_flags );
    
    /// Destructor
    ~Descriptor();

    /// \return The vk::DescriptorSetLayoutBinding for this UniformBuffer
    const vk::DescriptorSetLayoutBinding getVkDescriptorSetLayoutBinding();

    const vk::DescriptorPoolSize getVkDescriptorPoolSize();

    virtual void appendVkDescriptorSets( 
      std::vector< vk::WriteDescriptorSet >& write_descriptor_sets,
      std::vector< vk::CopyDescriptorSet >& copy_descriptor_sets,
      const vk::DescriptorSet& destination_set ) = 0;

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

}

#endif /* DESCRIPTOR_H */