#ifndef INDEXATTRIBUTE_CPP
#define INDEXATTRIBUTE_CPP

#include <VulkanTest/IndexAttribute.h>

template< typename T >
VulkanTest::IndexAttribute< T >::IndexAttribute( const T* data, size_t num_elements ) 
  : Attribute( num_elements, sizeof( T ), vk::BufferUsageFlagBits::eIndexBuffer ) {
  updateBuffer( data, sizeof( T ) * num_elements );
}

template< typename T >
VulkanTest::IndexAttribute< T >::~IndexAttribute() {
}

#endif