// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef BUFFER_H
#define BUFFER_H

#include <VulkanEngine/StagedBufferDestination.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#endif
#include <vk_mem_alloc.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Class which represents a single buffer using vk::Buffer.
class Buffer : public StagedBufferDestination {
public:
  /// Constructor.
  /// Creates the buffer given the parameters.
  /// \param _data_size The total size of the data in the buffer.
  /// \param usage_flags vk::BufferUsageFlags for this buffer.
  /// \param memory_property_flags vk::MemoryPropertyFlags for this buffer.
  /// \param vma_memory_usage VmaMemoryUsage flags to use when allocating.
  Buffer(size_t _data_size, vk::BufferUsageFlags usage_flags,
         vk::MemoryPropertyFlags memory_property_flags,
         VmaMemoryUsage vma_memory_usage);

  /// Destructor
  ~Buffer();

  /// \return The vk::Buffer instance of this Attribute.
  const vk::Buffer getVkBuffer() const;

  /// Overriden to handle tranferring data from a StagedBuffer to this buffer.
  /// \param command_buffers The command buffer to insert the command into.
  /// \param source_buffer The source vk::Buffer in the StagedBuffer.
  virtual void insertTransferCommand(const vk::CommandBuffer &command_buffer,
                                     const vk::Buffer &source_buffer);

  /// Override to return the required data size of the staging buffer in order
  /// to transfer all data to this buffer. \return The data size for the staging
  /// buffer.
  virtual size_t getStagingBufferSize() const;

private:
  /// Creates the buffer given the parameters.
  /// \param usage_flags vk::BufferUsageFlags for this buffer.
  /// \param memory_property_flags vk::MemoryPropertyFlags for this buffer.
  /// \param vma_memory_usage VmaMemoryUsage flags to use when allocating.
  void createBuffer(vk::BufferUsageFlags usage_flags,
                    vk::MemoryPropertyFlags memory_property_flags,
                    VmaMemoryUsage vma_memory_usage);

  /// vk::Buffer instance which represents this buffer.
  vk::Buffer vk_buffer;

protected:
  /// The size of the data.
  size_t data_size;
};

} // namespace VulkanEngine

#endif /* BUFFER_H */