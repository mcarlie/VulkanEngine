#ifndef STAGEDBUFFER_CPP
#define STAGEDBUFFER_CPP

#include <VulkanTest/StagedBuffer.h>

template< class DestinationClass >
template< class ... DestinationClassArgs >
VulkanTest::StagedBuffer< DestinationClass >::StagedBuffer( DestinationClassArgs ... args ) 
  : source_buffer( getStagingBufferSize(),
    vk::BufferUsageFlagBits::eTransferSrc,
    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    VMA_MEMORY_USAGE_CPU_ONLY ), DestinationClass( args ... ) {
}

template< class DestinationClass >
VulkanTest::StagedBuffer< DestinationClass >::~StagedBuffer() {
}

template< class DestinationClass >
void VulkanTest::StagedBuffer< DestinationClass >::transferBuffer( const vk::CommandBuffer& command_buffer ) {

  auto command_buffer_info = vk::CommandBufferAllocateInfo()
    .setCommandBufferCount( 1 )
    .setCommandPool( VulkanManager::getInstance()->getVkCommandPool() )
    .setLevel( vk::CommandBufferLevel::ePrimary );

  auto command_buffers = VulkanManager::getInstance()->getVkDevice().allocateCommandBuffers( command_buffer_info );
  if( command_buffers.empty() ){
    throw std::runtime_error( "Could not allocate command buffer for buffer transfer!" );
  }

  auto command_buffer_begin_info = vk::CommandBufferBeginInfo()
    .setFlags( vk::CommandBufferUsageFlagBits::eOneTimeSubmit );

  command_buffers[0].begin( command_buffer_begin_info );
  insertTransferCommand( command_buffers[0], source_buffer.getVkBuffer() );
  command_buffers[0].end();

  auto submit_info = vk::SubmitInfo()
    .setCommandBufferCount( static_cast< uint32_t >( command_buffers.size() ) )
    .setPCommandBuffers( command_buffers.data() );

  VulkanManager::getInstance()->getVkGraphicsQueue().submit( submit_info, nullptr );
  VulkanManager::getInstance()->getVkGraphicsQueue().waitIdle();

  VulkanManager::getInstance()->getVkDevice().freeCommandBuffers( VulkanManager::getInstance()->getVkCommandPool(), command_buffers );

}

template< class DestinationClass >
void VulkanTest::StagedBuffer< DestinationClass >::updateBuffer( const void* _data, size_t _data_size ) {
  source_buffer.updateBuffer( _data, _data_size );
}

#endif /* STAGEDBUFFER_CPP */