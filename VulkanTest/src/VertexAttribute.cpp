#ifndef VERTEXATTRIBUTE_CPP
#define VERTEXATTRIBUTE_CPP

#include <VulkanTest/VertexAttribute.h>

template< typename T, int elements_x, int elements_y >
VulkanTest::VertexAttribute< T, elements_x, elements_y >::VertexAttribute( 
  const std::vector< Eigen::Matrix< T, elements_x, elements_y > >& _data, uint32_t _binding ) 
  : binding( _binding ), Attribute() {

  num_elements = static_cast< uint32_t >( _data.size() );

  createBuffer( 
    _data.data(),
    sizeof( Eigen::Matrix< T, elements_x, elements_y > ) * _data.size(),
    vk::BufferUsageFlagBits::eVertexBuffer );

}

template< typename T, int elements_x, int elements_y >
VulkanTest::VertexAttribute< T, elements_x, elements_y >::~VertexAttribute() {
}

template< typename T, int elements_x, int elements_y >
const vk::VertexInputBindingDescription VulkanTest::VertexAttribute< T, elements_x, elements_y >::getVkVertexInputBindingDescription() {

  return vk::VertexInputBindingDescription()
    .setBinding( binding )
    .setInputRate( vk::VertexInputRate::eVertex )
    .setStride( sizeof( Eigen::Matrix< T, elements_x, elements_y > ) );

}

#endif /* VERTEXATTRIBUTE_CPP */