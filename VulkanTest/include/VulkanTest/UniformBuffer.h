#ifndef UNIFORMBUFFER_H
#define UNIFORMBUFFER_H

#include <VulkanTest/Buffer.h>

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  /// Represents a uniform buffer
  /// \tparam The type which represents the data
  template< typename T >
  class UniformBuffer : public Buffer {

  public:

    /// Constructor
    /// \param _binding The binding index
    /// \param _array_size For arrays, specify the array size
    /// \param _stage_flags Specify which shader stages will access the buffer
    UniformBuffer( 
      uint32_t _binding,
      uint32_t _array_size = 1,
      vk::ShaderStageFlags _stage_flags = vk::ShaderStageFlagBits::eAllGraphics );
    
    /// Destructor
    ~UniformBuffer();

    void updateUniformBuffer( const T* data, size_t array_size = 1 );

    /// \return The vk::DescriptorSetLayoutBinding for this UniformBuffer
    const vk::DescriptorSetLayoutBinding getVkDescriptorSetLayoutBinding();

  private:

    /// The binding index
    uint32_t binding;

    /// For arrays, specifies the array size
    uint32_t array_size;

    // Specifies which shader stages will access the buffer
    vk::ShaderStageFlags stage_flags;

  };

}

#include <UniformBuffer.cpp>

#endif /* UNIFORMBUFFER_H */