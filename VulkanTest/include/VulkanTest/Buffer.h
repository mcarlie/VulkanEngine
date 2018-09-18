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

    /// Handle allocation of buffer memory
    /// \param buffer The buffer to allocate memory for
    /// \param flags Flags specifying desired features of the allocated memory
    const vk::DeviceMemory allocateBufferMemory( const vk::Buffer& buffer, vk::MemoryPropertyFlags flags );

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

    /// \param type_filter The vk::MemoryRequirements::memoryTypeBits member
    /// \param flags vk::MemoryPropertyFlags for the desired index
    /// \return The memory type index for vk::MemoryAllocateInfo based on desired features
    uint32_t findMemoryTypeIndex( uint32_t type_filter, vk::MemoryPropertyFlags flags );

    /// The size of the data
    size_t data_size;

  };

}

#endif /* BUFFER_H */