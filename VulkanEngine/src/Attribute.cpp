#include <VulkanEngine/Attribute.h>

VulkanEngine::Attribute::Attribute( size_t _num_elements, size_t _element_size, vk::BufferUsageFlags flags ) 
  : StagedBuffer< Buffer >( 
    _num_elements * _element_size,
    flags | vk::BufferUsageFlagBits::eTransferDst,
    vk::MemoryPropertyFlagBits::eDeviceLocal, 
    VMA_MEMORY_USAGE_GPU_ONLY ),
  num_elements( _num_elements ),
  element_size( _element_size ) {

}

VulkanEngine::Attribute::~Attribute() {
}

size_t VulkanEngine::Attribute::getNumElements() {
  return num_elements;
}
