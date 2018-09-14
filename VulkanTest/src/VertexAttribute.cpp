#ifndef VERTEXATTRIBUTE_CPP
#define VERTEXATTRIBUTE_CPP

#include <VulkanTest/VertexAttribute.h>

template< typename T >
VulkanTest::VertexAttribute< T >::VertexAttribute( 
  const std::vector< T >& data, uint32_t _binding ) 
  : binding( _binding ), Attribute() {

  num_elements = static_cast< uint32_t >( data.size() );

  createBuffer( 
    sizeof( T ) * data.size(),
    vk::BufferUsageFlagBits::eVertexBuffer );

  updateBuffer( data.data(), sizeof( T ) * data.size() );

}

template< typename T >
VulkanTest::VertexAttribute< T >::~VertexAttribute() {
}

template< typename T >
const vk::VertexInputBindingDescription VulkanTest::VertexAttribute< T >::getVkVertexInputBindingDescription() {

  return vk::VertexInputBindingDescription()
    .setBinding( binding )
    .setInputRate( vk::VertexInputRate::eVertex )
    .setStride( sizeof( T ) );

}

#endif /* VERTEXATTRIBUTE_CPP */