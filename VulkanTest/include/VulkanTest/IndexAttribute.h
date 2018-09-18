#ifndef INDEXATTRIBUTE_H
#define INDEXATTRIBUTE_H

#include <VulkanTest/Attribute.h>

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  /// Represents an index attribute for indexed rendering
  /// \tparam T The type to use for index elements
  template< typename T >
  class IndexAttribute : public Attribute {

  public:

    /// Constructor
    /// \param _data The index data
    IndexAttribute( const T* data, size_t num_elements );

    /// Destructor
    ~IndexAttribute();

  };

}

#include <IndexAttribute.cpp>

#endif /* INDEXATTRIBUTE_H */