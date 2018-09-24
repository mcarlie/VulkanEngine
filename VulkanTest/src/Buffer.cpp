#include <VulkanTest/Buffer.h>
#include <VulkanTest/VulkanManager.h>

VulkanTest::Buffer::Buffer( 
  size_t _data_size,
  vk::BufferUsageFlags usage_flags,
  vk::MemoryPropertyFlags memory_property_flags,
  VmaMemoryUsage vma_memory_usage ) : data_size( _data_size ) {
  createBuffer( usage_flags, memory_property_flags, vma_memory_usage );
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

void VulkanTest::Buffer::createBuffer( 
  vk::BufferUsageFlags usage_flags,
  vk::MemoryPropertyFlags memory_property_flags,
  VmaMemoryUsage vma_memory_usage ) {

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
    &vma_allocation,
    nullptr );

  if( result != VK_SUCCESS ) {
    throw std::runtime_error( "Could not create buffer!" );
  }

  vk_buffer = buffer;

}

void VulkanTest::Buffer::insertTransferCommand( const vk::CommandBuffer& command_buffer, const vk::Buffer& source_buffer ) {

  auto buffer_copy = vk::BufferCopy()
    .setSrcOffset( 0 )
    .setDstOffset( 0 )
    .setSize( data_size );

  command_buffer.copyBuffer( source_buffer, vk_buffer, buffer_copy );
    
}

size_t VulkanTest::Buffer::getStagingBufferSize() {
  return data_size;
}
