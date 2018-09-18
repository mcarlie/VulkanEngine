#ifndef UNIFORMBUFFER_CPP
#define UNIFORMBUFFER_CPP

#include <VulkanTest/UniformBuffer.h>

template< typename T >
VulkanTest::UniformBuffer< T >::UniformBuffer( uint32_t _binding, uint32_t _array_size, vk::ShaderStageFlags _stage_flags ) 
  : Buffer(), binding( _binding ), array_size( _array_size ), stage_flags( _stage_flags ) {

  vk_buffer = createBuffer( sizeof( T ) * array_size, vk::BufferUsageFlagBits::eUniformBuffer );
  vk_device_memory = allocateBufferMemory( 
    vk_buffer,
    vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible );

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

template< typename T >
void VulkanTest::UniformBuffer< T >::updateUniformBuffer( const T* data, size_t array_size ) {

  updateBuffer( data, sizeof( T ) * array_size );

}

#endif /* UNIFORMBUFFER_CPP */