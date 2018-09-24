#include <VulkanTest/BufferBase.h>
#include <VulkanTest/VulkanManager.h>

void VulkanTest::BufferBase::updateBuffer( const void* _data, size_t _data_size ) {

  void* mapped_memory = nullptr;
  const VmaAllocator& vma_allocator = VulkanManager::getInstance()->getVmaAllocator();
  vmaMapMemory( vma_allocator, vma_allocation, &mapped_memory );
  std::memcpy( mapped_memory, _data, _data_size );
  vmaUnmapMemory( vma_allocator, vma_allocation );

}