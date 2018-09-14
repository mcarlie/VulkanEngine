#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <VulkanTest/Buffer.h>

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  /// Abstract base class for shader attributes
  class Attribute : public Buffer {

  public:

    /// Constructor
    Attribute();
    
    /// Destructor
    ~Attribute();

    /// \return The number of elements in this Attribute
    uint32_t getNumElements();

  protected:

    /// The number of elements in this attribute
    uint32_t num_elements;

  };

}

#endif /* ATTRIBUTE_H */