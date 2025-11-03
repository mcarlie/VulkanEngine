// Copyright (c) 2025 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
    const vk::CommandBuffer& command_buffer) {
  const vk::CommandBuffer& command_buffer_to_use =
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
    const void* _data, size_t _data_size) {
  source_buffer.updateBuffer(_data, _data_size);
}

#endif /* STAGEDBUFFER_CPP */
