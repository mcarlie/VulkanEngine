#include <VulkanTest/Buffer.h>
#include <VulkanTest/VulkanManager.h>

VulkanTest::Buffer::Buffer() {
}

VulkanTest::Buffer::~Buffer() {

  vmaDestroyBuffer( 
    VulkanManager::getInstance()->getVmaAllocator(),
    static_cast< VkBuffer >( vk_buffer ), 
    vma_allocation );

}

const vk::Buffer& VulkanTest::Buffer::getVkBuffer() {
  return vk_buffer;
}

const vk::Buffer VulkanTest::Buffer::createBuffer( 
  size_t _data_size,
  vk::BufferUsageFlags usage_flags,
  vk::MemoryPropertyFlags memory_property_flags,
  VmaMemoryUsage vma_memory_usage,
  VmaAllocation& _vma_allocation ) {

  data_size = _data_size;

  auto vulkan_manager = VulkanManager::getInstance();
  const vk::Device& vk_device = vulkan_manager->getVkDevice();

  auto buffer_info = vk::BufferCreateInfo()
    .setSize( data_size )
    .setUsage( usage_flags )
    .setSharingMode( vk::SharingMode::eExclusive );

  VmaAllocationCreateInfo vma_allocation_create_info = {};
  vma_allocation_create_info.usage = vma_memory_usage;
  vma_allocation_create_info.requiredFlags = static_cast< VkMemoryPropertyFlags >( memory_property_flags );

  vk::Buffer buffer;
  VkBufferCreateInfo buffer_create_info_c_handle = static_cast< VkBufferCreateInfo >( buffer_info );

  auto result = vmaCreateBuffer( 
    vulkan_manager->getVmaAllocator(),
    &buffer_create_info_c_handle,
    &vma_allocation_create_info,
    reinterpret_cast< VkBuffer* >( &buffer ),
    &_vma_allocation,
    nullptr );

  if( result != VK_SUCCESS ) {
    throw std::runtime_error( "Could not create buffer!" );
  }

  return buffer;

}

void VulkanTest::Buffer::updateBuffer( const void* data, size_t _data_size ) {

  updateBuffer( data, data_size, vma_allocation );

}

void VulkanTest::Buffer::updateBuffer( const void* data, size_t _data_size, VmaAllocation& _vma_allocation ) {

  void* mapped_memory = nullptr;
  vmaMapMemory( VulkanManager::getInstance()->getVmaAllocator(), _vma_allocation, &mapped_memory );
  memcpy( mapped_memory, data, _data_size );
  vmaUnmapMemory( VulkanManager::getInstance()->getVmaAllocator(), _vma_allocation );

}
