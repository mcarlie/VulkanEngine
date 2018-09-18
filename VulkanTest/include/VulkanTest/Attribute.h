#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <VulkanTest/Buffer.h>

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  /// Abstract base class for shader attributes
  class Attribute : public Buffer {

  public:

    /// Constructor
    Attribute( size_t _num_elements, size_t _element_size, vk::BufferUsageFlags flags );
    
    /// Destructor
    ~Attribute();

    /// \return The number of elements in this Attribute
    size_t getNumElements();

    /// Start transfer of buffer data from staging buffer to vertex buffer
    void transferBuffer();

  protected:

    /// Copy the data to the buffer
    /// Overriden to update staging buffer
    /// \param data Pointer to the data
    /// \param data_size The size of the data in bytes
    virtual void updateBuffer( const void* data, size_t _data_size );

    /// The number of elements in this attribute
    size_t num_elements;

    /// The data size of the elements which this attribute is represented by
    size_t element_size;

  private:

    /// Buffer used for staging before transferring to device local location
    vk::Buffer vk_staging_buffer;

    VmaAllocation vma_staging_allocation;

  };

}

#endif /* ATTRIBUTE_H */