#include <VulkanTest/Attribute.h>
#include <VulkanTest/VulkanManager.h>

VulkanTest::Attribute::Attribute( size_t _num_elements, size_t _element_size, vk::BufferUsageFlags flags ) 
  : Buffer(), num_elements( _num_elements ), element_size( _element_size ) {

  vk_staging_buffer = createBuffer( 
    num_elements * element_size,
    vk::BufferUsageFlagBits::eTransferSrc,
    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    VMA_MEMORY_USAGE_CPU_ONLY,
    vma_staging_allocation );

  vk_buffer = createBuffer( 
    num_elements * element_size,
    flags | vk::BufferUsageFlagBits::eTransferDst,
    vk::MemoryPropertyFlagBits::eDeviceLocal,
    VMA_MEMORY_USAGE_GPU_ONLY,
    vma_allocation );

}

VulkanTest::Attribute::~Attribute() {

  vmaDestroyBuffer( 
    VulkanManager::getInstance()->getVmaAllocator(),
    static_cast< VkBuffer >( vk_staging_buffer ), 
    vma_staging_allocation );

}

size_t VulkanTest::Attribute::getNumElements() {
  return num_elements;
}

void VulkanTest::Attribute::transferBuffer() {

  auto command_buffer_info = vk::CommandBufferAllocateInfo()
    .setCommandBufferCount( 1 )
    .setCommandPool( VulkanManager::getInstance()->getVkCommandPool() )
    .setLevel( vk::CommandBufferLevel::ePrimary );

  auto command_buffers = VulkanManager::getInstance()->getVkDevice().allocateCommandBuffers( command_buffer_info );
  if( command_buffers.empty() ){
    throw std::runtime_error( "Could not allocate command buffer for buffer transfer!" );
  }

  auto buffer_copy = vk::BufferCopy()
    .setSrcOffset( 0 )
    .setDstOffset( 0 )
    .setSize( element_size * num_elements );

  auto command_buffer_begin_info = vk::CommandBufferBeginInfo()
    .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );

  command_buffers[0].begin( command_buffer_begin_info );
  command_buffers[0].copyBuffer( vk_staging_buffer, vk_buffer, buffer_copy );
  command_buffers[0].end();

  auto submit_info = vk::SubmitInfo()
    .setCommandBufferCount( static_cast< uint32_t >( command_buffers.size() ) )
    .setPCommandBuffers( command_buffers.data() );

  VulkanManager::getInstance()->getVkGraphicsQueue().submit( submit_info, nullptr );
  VulkanManager::getInstance()->getVkGraphicsQueue().waitIdle();

  auto device = VulkanManager::getInstance()->getVkDevice();
  vmaDestroyBuffer( 
    VulkanManager::getInstance()->getVmaAllocator(),
    static_cast< VkBuffer >( vk_staging_buffer ),
    vma_staging_allocation );

  device.freeCommandBuffers( VulkanManager::getInstance()->getVkCommandPool(), command_buffers );

}

void VulkanTest::Attribute::updateBuffer( const void* data, size_t _data_size ) {

  Buffer::updateBuffer( data, _data_size, vma_staging_allocation );

}