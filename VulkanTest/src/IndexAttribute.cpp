#ifndef INDEXATTRIBUTE_CPP
#define INDEXATTRIBUTE_CPP

#include <VulkanTest/IndexAttribute.h>

template< typename T >
VulkanTest::IndexAttribute< T >::IndexAttribute( const std::vector< T >& _data ) {

  num_elements = _data.size();

  createBuffer( _data.data(), sizeof( T ) * _data.size(), vk::BufferUsageFlagBits::eIndexBuffer );

}

template< typename T >
VulkanTest::IndexAttribute< T >::~IndexAttribute() {
}

#endif