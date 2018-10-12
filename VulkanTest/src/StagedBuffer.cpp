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

  const vk::CommandBuffer& command_buffer_to_use = command_buffer ? command_buffer : single_use_command_buffer;
  bool created_single_use_command_buffer = false;
  if( !command_buffer_to_use ) {
    created_single_use_command_buffer = true;
    beginSingleUsageCommandBuffer();
  }

  insertTransferCommand( command_buffer_to_use, source_buffer.getVkBuffer() );

  if( created_single_use_command_buffer ){
    endSingleUsageCommandBuffer();
  }

}

template< class DestinationClass >
void VulkanTest::StagedBuffer< DestinationClass >::updateBuffer( const void* _data, size_t _data_size ) {
  source_buffer.updateBuffer( _data, _data_size );
}

#endif /* STAGEDBUFFER_CPP */