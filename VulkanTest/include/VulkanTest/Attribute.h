#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  /// Abstract base class for geometry attributes
  class Attribute {

  public:

    /// Constructor
    Attribute();
    
    /// Destructor
    ~Attribute();

    /// \return The number of elements in this Attribute
    uint32_t getNumElements();

    /// \return The vk::Buffer instance of this Attribute
    const vk::Buffer& getVkBuffer();

  protected:

    /// Create a buffer and copy the data to the buffer
    /// \param data Pointer to the data
    /// \param data_size The size of the data in number of bytes
    /// \param usage_flags vk::BufferUsageFlags for the buffer
    void createBuffer( const void* data, size_t data_size, vk::BufferUsageFlags usage_flags );

    /// The number of elements in this attribute
    uint32_t num_elements;

    /// vk::Buffer instance initialized in createBuffer()
    vk::Buffer vk_buffer;
    
    /// vk::DeviceMemory instance used handle buffer memory
    vk::DeviceMemory vk_device_memory;

  };

}

#endif /* ATTRIBUTE_H */