#ifndef INDEXATTRIBUTE_CPP
#define INDEXATTRIBUTE_CPP

#include <VulkanEngine/IndexAttribute.h>

template< typename T >
VulkanEngine::IndexAttribute< T >::IndexAttribute( const T* data, size_t num_elements ) 
  : Attribute( num_elements, sizeof( T ), vk::BufferUsageFlagBits::eIndexBuffer ) {
  updateBuffer( data, sizeof( T ) * num_elements );
}

template< typename T >
VulkanEngine::IndexAttribute< T >::~IndexAttribute() {
}

#endif