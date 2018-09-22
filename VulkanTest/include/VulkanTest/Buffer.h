#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

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

    /// Create a buffer with appropriate settings
    /// \param data Pointer to the data
    /// \param data_size The size of the data in number of bytes
    /// \param usage_flags vk::BufferUsageFlags for the buffer
    /// \return The created buffer
    virtual const vk::Buffer createBuffer( 
      size_t _data_size,
      vk::BufferUsageFlags usage_flags,
      vk::MemoryPropertyFlags memory_property_flags,
      VmaMemoryUsage vma_memory_usage,
      VmaAllocation& _vma_allocation );

    /// Copy the data to the buffer
    /// \param data Pointer to the data
    /// \param data_size The size of the data in bytes
    virtual void updateBuffer( const void* data, size_t _data_size );

    /// Copy the data to the buffer
    /// \param data Pointer to the data
    /// \param data_size The size of the data in bytes
    /// \param _vma_allocation The VmaAllocation belonging to the buffer
    void updateBuffer( const void* data, size_t _data_size, VmaAllocation& _vma_allocation );

    /// vk::Buffer instance which represents this buffer
    vk::Buffer vk_buffer;
    
    /// VmaAllocation instance belonging to vk_buffer;
    VmaAllocation vma_allocation;

  private:

    /// The size of the data
    size_t data_size;

  };

}

#endif /* BUFFER_H */