#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

  /// Base class for any classes which interface with the Vulkan resource descriptor concept.
  class Descriptor {

  public:

    /// Constructor.
    Descriptor( uint32_t _binding, uint32_t _desciptor_count, vk::DescriptorType _vk_descriptor_type, vk::ShaderStageFlags _vk_shader_stage_flags );
    
    /// Destructor.
    ~Descriptor();

    /// \return The vk::DescriptorSetLayoutBinding for this UniformBuffer.
    const vk::DescriptorSetLayoutBinding getVkDescriptorSetLayoutBinding();

    /// \return A vk::DescriptorPoolSize describing this descriptor for creating a descriptor pool.
    const vk::DescriptorPoolSize getVkDescriptorPoolSize();

    /// Appends any write descriptor sets or copy descriptor sets which belong
    /// to this Descriptor to the given vectors.
    /// \param [out] write_descriptor_sets Any vk::WriteDescriptorSets of the descriptor should be appended to this vector.
    /// \param [out] copy_descriptor_sets Any vk::CopyDescriptorSets of the descriptor should be appended to this vector.
    /// \param destination_set The destination descriptor set of the write and copy descriptor sets.
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