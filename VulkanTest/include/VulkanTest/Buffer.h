#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.hpp>

namespace VulkanTest {

  /// Base class for all types of buffers
  class Buffer {

  public:

    /// Constructor
    Buffer();

    /// Destructor
    ~Buffer();
  
    /// \return The vk::Buffer instance of this Attribute
    const vk::Buffer& getVkBuffer();

  protected:

    /// Create a buffer with the appropriate settings
    /// \param data Pointer to the data
    /// \param data_size The size of the data in number of bytes
    /// \param usage_flags vk::BufferUsageFlags for the buffer
    virtual void createBuffer( size_t data_size, vk::BufferUsageFlags usage_flags );

    /// Copy the data to the buffer
    /// Default implementation simply memcpys the data to the buffer
    /// \param data Pointer to the data
    /// \param data_size The size of the data in bytes
    virtual void updateBuffer( const void* data, size_t data_size );

    /// vk::Buffer instance which represents this buffer
    vk::Buffer vk_buffer;
    
    /// vk::DeviceMemory instance used handle buffer memory
    vk::DeviceMemory vk_device_memory;

  protected:

    int32_t findMemoryTypeIndex( uint32_t type_filter, vk::MemoryPropertyFlags flags );

  };

}

#endif /* BUFFER_H */