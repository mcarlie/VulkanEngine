#include <VulkanTest/Attribute.h>
#include <VulkanTest/VulkanManager.h>

VulkanTest::Attribute::Attribute( size_t _num_elements, size_t _element_size, vk::BufferUsageFlags flags ) 
  : StagedBuffer< Buffer >( 
    _num_elements * _element_size,
    flags | vk::BufferUsageFlagBits::eTransferDst,
    vk::MemoryPropertyFlagBits::eDeviceLocal, 
    VMA_MEMORY_USAGE_GPU_ONLY ),
  num_elements( _num_elements ),
  element_size( _element_size ) {

}

VulkanTest::Attribute::~Attribute() {
}

size_t VulkanTest::Attribute::getNumElements() {
  return num_elements;
}