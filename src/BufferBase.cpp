// Copyright (c) 2025 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <VulkanEngine/BufferBase.h>
#include <VulkanEngine/VulkanManager.h>

void VulkanEngine::BufferBase::updateBuffer(const void* _data,
                                            size_t _data_size) {
  void* mapped_memory = nullptr;
  const VmaAllocator& vma_allocator =
      VulkanManager::getInstance().getDevice()->getVmaAllocator();
  auto result = vmaMapMemory(vma_allocator, vma_allocation, &mapped_memory);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Could not map memory for buffer!");
  }
  std::memcpy(mapped_memory, _data, _data_size);
  vmaUnmapMemory(vma_allocator, vma_allocation);
}
