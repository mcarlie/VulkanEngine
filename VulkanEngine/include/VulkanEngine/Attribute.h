#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <VulkanEngine/StagedBuffer.h>

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

  /// Abstract base class for shader attributes
  class Attribute : public StagedBuffer< Buffer > {

  public:

    /// Constructor.
    /// \param _num_elements The number of elements in this attribute buffer
    /// \param _element_size The size of a single element in the attribute buffer
    /// \param flags The attribute's vk::BufferUsageFlags
    Attribute( size_t _num_elements, size_t _element_size, vk::BufferUsageFlags flags );
    
    /// Destructor.
    ~Attribute();

    /// \return The number of elements in this Attribute.
    size_t getNumElements();

  protected:

    /// The number of elements in this attribute.
    size_t num_elements;

    /// The data size of the elements which this attribute is represented by.
    size_t element_size;

  };

}

#endif /* ATTRIBUTE_H */