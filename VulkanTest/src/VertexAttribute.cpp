#ifndef VERTEXATTRIBUTE_CPP
#define VERTEXATTRIBUTE_CPP

#include <VulkanTest/VertexAttribute.h>

template< typename T >
VulkanTest::VertexAttribute< T >::VertexAttribute( 
  const T* data, size_t _num_elements, uint32_t _location, vk::Format _format ) : 
  Attribute( _num_elements, sizeof( T ), vk::BufferUsageFlagBits::eVertexBuffer ),
  location( _location ),
  format( _format ) {
  
  updateBuffer( data, sizeof( T ) * num_elements );

}

template< typename T >
VulkanTest::VertexAttribute< T >::~VertexAttribute() {
}

template< typename T >
const vk::VertexInputBindingDescription VulkanTest::VertexAttribute< T >::getVkVertexInputBindingDescription() {

  return vk::VertexInputBindingDescription()
    .setBinding( 0 )
    .setInputRate( vk::VertexInputRate::eVertex )
    .setStride( sizeof( T ) );

}

template< typename T >
const vk::VertexInputAttributeDescription VulkanTest::VertexAttribute< T >::getVkVertexInputAttributeDescriptions() {

  return vk::VertexInputAttributeDescription()
    .setBinding( 0 )
    .setLocation( location )
    .setFormat( getVkFormat() )
    .setOffset( 0 );
}

template< typename T >
vk::Format VulkanTest::VertexAttribute< T >::getVkFormat() {
  return format;
}

#endif /* VERTEXATTRIBUTE_CPP */