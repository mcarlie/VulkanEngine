#ifndef BUFFERBASE_H
#define BUFFERBASE_H

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#include <vk_mem_alloc.h>
#pragma clang diagnostic pop
#endif

namespace VulkanEngine {

/// Base class for various classes which represent data buffers on the GPU/CPU.
class BufferBase {
public:
  /// Copy the data to the buffer.
  /// \param data Pointer to the data.
  /// \param data_size The size of the data in bytes.
  virtual void updateBuffer(const void *_data, size_t _data_size);

protected:
  /// VmaAllocation used to handle allocation with Vulkan Memory Allocator
  /// library.
  VmaAllocation vma_allocation;
};

} // namespace VulkanEngine

#endif /* BUFFERBASE_H */