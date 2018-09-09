#ifndef INDEXATTRIBUTE_CPP
#define INDEXATTRIBUTE_CPP

#include <VulkanTest/IndexAttribute.h>

template< typename T >
VulkanTest::IndexAttribute< T >::IndexAttribute( const std::vector< T >& data ) {

  num_elements = static_cast< uint32_t >( data.size() );
  createBuffer( data.data(), sizeof( T ) * data.size(), vk::BufferUsageFlagBits::eIndexBuffer );

}

template< typename T >
VulkanTest::IndexAttribute< T >::~IndexAttribute() {
}

#endif