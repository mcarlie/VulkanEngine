#ifndef INDEXATTRIBUTE_H
#define INDEXATTRIBUTE_H

#include <VulkanTest/Attribute.h>

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  template< typename T >
  class IndexAttribute : public Attribute {

  public:

    /// Constructor
    IndexAttribute( const std::vector< T >& _data );

    /// Destructor
    ~IndexAttribute();

  };

}

#include <IndexAttribute.cpp>

#endif /* INDEXATTRIBUTE_H */