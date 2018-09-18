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

const vk::Buffer VulkanTest::Buffer::createBuffer( 
  size_t _data_size,
  vk::BufferUsageFlags usage_flags ) {

  data_size = _data_size;

  auto vulkan_manager = VulkanManager::getInstance();
  vk::Device& vk_device = vulkan_manager->getVkDevice();

  auto buffer_info = vk::BufferCreateInfo()
    .setSize( data_size )
    .setUsage( usage_flags )
    .setSharingMode( vk::SharingMode::eExclusive );

  vk::Buffer buffer = vk_device.createBuffer( buffer_info );
  if( !buffer ) {
    throw std::runtime_error( "Could not create buffer!" );
  }

  return buffer;

}

void VulkanTest::Buffer::updateBuffer( const void* data, size_t data_size ) {

  updateBuffer( data, data_size, vk_device_memory );

}

void VulkanTest::Buffer::updateBuffer( 
  const void* data,
  size_t data_size,
  vk::DeviceMemory device_memory ) {

  auto& device = VulkanManager::getInstance()->getVkDevice();
  void* data_ptr = device.mapMemory( device_memory, 0, data_size );
  std::memcpy( data_ptr, data, data_size );
  device.unmapMemory( device_memory );

}

const vk::DeviceMemory VulkanTest::Buffer::allocateBufferMemory( const vk::Buffer& buffer, vk::MemoryPropertyFlags flags ) {

  auto vulkan_manager = VulkanManager::getInstance();
  vk::Device& vk_device = vulkan_manager->getVkDevice();

  vk::MemoryRequirements vk_memory_requirements = vk_device.getBufferMemoryRequirements( buffer );

  auto vk_allocate_info = vk::MemoryAllocateInfo()
    .setAllocationSize( vk_memory_requirements.size )
    .setMemoryTypeIndex( findMemoryTypeIndex( vk_memory_requirements.memoryTypeBits,
      flags ) );

  vk::DeviceMemory device_memory = vk_device.allocateMemory( vk_allocate_info );
  if( !device_memory ) {
    throw std::runtime_error( "Could not allocate memory for buffer!" );
  }
  
  vk_device.bindBufferMemory( buffer, device_memory, 0 );

  return device_memory;

}

uint32_t VulkanTest::Buffer::findMemoryTypeIndex( uint32_t type_filter, vk::MemoryPropertyFlags flags ) {

  auto& physical_device = VulkanManager::getInstance()->getVKPhysicalDevice();
  vk::PhysicalDeviceMemoryProperties vk_memory_properties = physical_device.getMemoryProperties();
  for( uint32_t i = 0; i < vk_memory_properties.memoryTypeCount; ++i ) {
    if( ( type_filter & ( 1 << i ) ) && ( vk_memory_properties.memoryTypes[i].propertyFlags & flags ) == flags ) {
      return i;
    }
  }

  throw std::runtime_error( "Could not find suitable memory type" );

}
