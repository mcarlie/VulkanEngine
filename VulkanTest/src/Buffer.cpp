#include <VulkanTest/Buffer.h>

#include <VulkanTest/VulkanManager.h>

VulkanTest::Buffer::Buffer() {
}

VulkanTest::Buffer::~Buffer() {

  auto device = VulkanManager::getInstance()->getVkDevice();
  device.destroyBuffer( vk_buffer );
  device.freeMemory( vk_device_memory );

}

const vk::Buffer& VulkanTest::Buffer::getVkBuffer() {

  return vk_buffer;

}

void VulkanTest::Buffer::createBuffer( size_t data_size, vk::BufferUsageFlags usage_flags ) {

  auto vulkan_manager = VulkanManager::getInstance();
  vk::Device& vk_device = vulkan_manager->getVkDevice();

  auto buffer_info = vk::BufferCreateInfo()
    .setSize( data_size )
    .setUsage( usage_flags )
    .setSharingMode( vk::SharingMode::eExclusive );

  vk_buffer = vk_device.createBuffer( buffer_info );
  vk::MemoryRequirements vk_memory_requirements = vk_device.getBufferMemoryRequirements( vk_buffer );

  auto vk_allocate_info = vk::MemoryAllocateInfo()
    .setAllocationSize( vk_memory_requirements.size )
    .setMemoryTypeIndex( vulkan_manager->findMemoryTypeIndex( vk_memory_requirements.memoryTypeBits,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent ) );

  vk_device_memory = vk_device.allocateMemory( vk_allocate_info );
  vk_device.bindBufferMemory( vk_buffer, vk_device_memory, 0 );

}

void VulkanTest::Buffer::updateBuffer( const void* data, size_t data_size ) {

  auto& device = VulkanManager::getInstance()->getVkDevice();
  void* data_ptr = device.mapMemory( vk_device_memory, 0, data_size );
  std::memcpy( data_ptr, data, data_size );
  device.unmapMemory( vk_device_memory );

}