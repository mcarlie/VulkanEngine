// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_BUFFERBASE_H_
#define INCLUDE_VULKANENGINE_BUFFERBASE_H_

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#endif
#include <vk_mem_alloc.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace VulkanEngine {

/// Base class for various classes which represent data buffers on the GPU/CPU.
class BufferBase {
 public:
  /// Copy the data to the buffer.
  /// \param data Pointer to the data.
  /// \param data_size The size of the data in bytes.
  virtual void updateBuffer(const void* _data, size_t _data_size);

 protected:
  /// VmaAllocation used to handle allocation with Vulkan Memory Allocator
  /// library.
  VmaAllocation vma_allocation;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_BUFFERBASE_H_
