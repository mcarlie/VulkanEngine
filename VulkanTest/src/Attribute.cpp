#include <VulkanTest/Attribute.h>
#include <VulkanTest/Renderer.h>

VulkanTest::Attribute::Attribute() : num_elements( 0 ) {
}

VulkanTest::Attribute::~Attribute() {
  auto device = Renderer::get()->getVkDevice();
  device.destroyBuffer( vk_buffer );
  device.freeMemory( vk_device_memory );
}

uint32_t VulkanTest::Attribute::getNumElements() {
  return num_elements;
}

const vk::Buffer& VulkanTest::Attribute::getVkBuffer() {
  return vk_buffer;
}

void VulkanTest::Attribute::createBuffer( const void* data, size_t data_size, vk::BufferUsageFlags usage_flags ) {

  auto renderer = Renderer::get();
  vk::Device& vk_device = renderer->getVkDevice();

  auto buffer_info = vk::BufferCreateInfo()
    .setSize( data_size )
    .setUsage( usage_flags )
    .setSharingMode( vk::SharingMode::eExclusive );

  vk_buffer = vk_device.createBuffer( buffer_info );
  vk::MemoryRequirements vk_memory_requirements = vk_device.getBufferMemoryRequirements( vk_buffer );

  auto vk_allocate_info = vk::MemoryAllocateInfo()
    .setAllocationSize( vk_memory_requirements.size )
    .setMemoryTypeIndex( renderer->findMemoryTypeIndex( vk_memory_requirements.memoryTypeBits,
      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent ) );

  vk_device_memory = vk_device.allocateMemory( vk_allocate_info );
  vk_device.bindBufferMemory( vk_buffer, vk_device_memory, 0 );

  void* data_ptr = vk_device.mapMemory( vk_device_memory, 0, data_size );
  std::memcpy( data_ptr, data, data_size );
  vk_device.unmapMemory( vk_device_memory );

}