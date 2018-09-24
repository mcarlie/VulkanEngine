#ifndef UNIFORMBUFFER_CPP
#define UNIFORMBUFFER_CPP

#include <VulkanTest/UniformBuffer.h>

template< typename T >
VulkanTest::UniformBuffer< T >::UniformBuffer( uint32_t _binding, uint32_t _array_size, vk::ShaderStageFlags _stage_flags ) 
  : Buffer( sizeof( T ) * _array_size, vk::BufferUsageFlagBits::eUniformBuffer,
    vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
    VMA_MEMORY_USAGE_CPU_TO_GPU ), 
  binding( _binding ), 
  array_size( _array_size ), 
  stage_flags( _stage_flags ) {
}

template< typename T >
VulkanTest::UniformBuffer< T >::~UniformBuffer() {
}

template< typename T >
const vk::DescriptorSetLayoutBinding VulkanTest::UniformBuffer< T >::getVkDescriptorSetLayoutBinding() {

  return vk::DescriptorSetLayoutBinding()
    .setBinding( binding )
    .setDescriptorCount( array_size )
    .setDescriptorType( vk::DescriptorType::eUniformBuffer )
    .setStageFlags( stage_flags );

}

#endif /* UNIFORMBUFFER_CPP */