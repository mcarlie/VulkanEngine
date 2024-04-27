#include <VulkanEngine/BufferBase.h>
#include <VulkanEngine/VulkanManager.h>

void VulkanEngine::BufferBase::updateBuffer(const void *_data,
                                            size_t _data_size) {
  void *mapped_memory = nullptr;
  const VmaAllocator &vma_allocator =
      VulkanManager::getInstance().getVmaAllocator();
  auto result = vmaMapMemory(vma_allocator, vma_allocation, &mapped_memory);
  if (result != VK_SUCCESS) {
    throw std::runtime_error("Could not map memory for buffer!");
  }
  std::memcpy(mapped_memory, _data, _data_size);
  vmaUnmapMemory(vma_allocator, vma_allocation);
}
