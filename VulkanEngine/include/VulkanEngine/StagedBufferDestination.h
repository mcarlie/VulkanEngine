#ifndef STAGEDBUFFERDESTINATION_H
#define STAGEDBUFFERDESTINATION_H

#include <VulkanEngine/BufferBase.h>
#include <VulkanEngine/SingleUsageCommandBuffer.h>

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Base class for classes which are capable of being targets of a StagedBuffer.
/// insertTransferCommand is called from StagedBuffer::tranferBuffer().
class StagedBufferDestination : public BufferBase,
                                protected SingleUsageCommandBuffer {

  /// Override to handle tranferring data from a StagedBuffer to this buffer.
  /// \param command_buffers The command buffer to insert the command into.
  /// \param source_buffer The source vk::Buffer in the StagedBuffer.
  virtual void insertTransferCommand(const vk::CommandBuffer &command_buffer,
                                     const vk::Buffer &source_buffer) = 0;

  /// Override to return the required data size of the staging buffer in order
  /// to transfer all data to this buffer. \return The data size for the staging
  /// buffer.
  virtual size_t getStagingBufferSize() = 0;
};

} // namespace VulkanEngine

#endif /* STAGEDBUFFERDESTINATION_H */