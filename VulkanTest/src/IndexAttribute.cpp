#ifndef INDEXATTRIBUTE_CPP
#define INDEXATTRIBUTE_CPP

#include <VulkanTest/IndexAttribute.h>

template< typename T >
VulkanTest::IndexAttribute< T >::IndexAttribute( const std::vector< T >& data ) {

  num_elements = static_cast< uint32_t >( data.size() );
  createBuffer( sizeof( T ) * num_elements, vk::BufferUsageFlagBits::eIndexBuffer );
  updateBuffer( data.data(), sizeof( T ) * num_elements );

}

template< typename T >
VulkanTest::IndexAttribute< T >::~IndexAttribute() {
}

#endif