#ifndef STAGEDBUFFER_CPP
#define STAGEDBUFFER_CPP

#include <VulkanEngine/StagedBuffer.h>

template <class DestinationClass>
template <class... DestinationClassArgs>
VulkanEngine::StagedBuffer<DestinationClass>::StagedBuffer(
    DestinationClassArgs... args)
    : DestinationClass(args...),
      source_buffer(this->getStagingBufferSize(),
                    vk::BufferUsageFlagBits::eTransferSrc,
                    vk::MemoryPropertyFlagBits::eHostVisible |
                        vk::MemoryPropertyFlagBits::eHostCoherent,
                    VMA_MEMORY_USAGE_CPU_ONLY) {}

template <class DestinationClass>
VulkanEngine::StagedBuffer<DestinationClass>::~StagedBuffer() {}

template <class DestinationClass>
void VulkanEngine::StagedBuffer<DestinationClass>::transferBuffer(
    const vk::CommandBuffer &command_buffer) {

  const vk::CommandBuffer &command_buffer_to_use =
      command_buffer ? command_buffer : this->single_use_command_buffer;
  bool created_single_use_command_buffer = false;
  if (!command_buffer_to_use) {
    created_single_use_command_buffer = true;
    this->beginSingleUsageCommandBuffer();
  }

  this->insertTransferCommand(command_buffer_to_use,
                              source_buffer.getVkBuffer());

  if (created_single_use_command_buffer) {
    this->endSingleUsageCommandBuffer();
  }
}

template <class DestinationClass>
void VulkanEngine::StagedBuffer<DestinationClass>::updateBuffer(
    const void *_data, size_t _data_size) {
  source_buffer.updateBuffer(_data, _data_size);
}

#endif /* STAGEDBUFFER_CPP */
