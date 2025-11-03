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

#ifndef UNIFORMBUFFER_CPP
#define UNIFORMBUFFER_CPP

#include <VulkanEngine/UniformBuffer.h>

#include <memory>

template <typename T>
VulkanEngine::UniformBuffer<T>::UniformBuffer(
    uint32_t _binding, uint32_t _array_size,
    vk::ShaderStageFlags _vk_shader_stage_flags)
    : Buffer(sizeof(T) * _array_size, vk::BufferUsageFlagBits::eUniformBuffer,
             vk::MemoryPropertyFlagBits::eHostCoherent |
                 vk::MemoryPropertyFlagBits::eHostVisible,
             VMA_MEMORY_USAGE_CPU_TO_GPU),
      Descriptor(_binding, _array_size, vk::DescriptorType::eUniformBuffer,
                 _vk_shader_stage_flags),
      array_size(_array_size) {}

template <typename T>
VulkanEngine::UniformBuffer<T>::~UniformBuffer() {}

template <typename T>
void VulkanEngine::UniformBuffer<T>::appendVkDescriptorSets(
    std::shared_ptr<std::vector<vk::WriteDescriptorSet>> write_descriptor_sets,
    std::shared_ptr<std::vector<vk::CopyDescriptorSet>> copy_descriptor_sets,
    const vk::DescriptorSet& destination_set) {
  vk_descriptor_buffer_infos.clear();

  for (uint32_t i = 0; i < array_size; ++i) {
    vk_descriptor_buffer_infos.push_back(
        vk::DescriptorBufferInfo()
            .setBuffer(getVkBuffer())
            .setOffset(sizeof(T) * static_cast<size_t>(i))
            .setRange(sizeof(T)));

    write_descriptor_sets->push_back(
        vk::WriteDescriptorSet()
            .setDstBinding(binding)
            .setDstArrayElement(i)
            .setDstSet(destination_set)
            .setDescriptorType(vk_descriptor_type)
            .setDescriptorCount(1)
            .setPBufferInfo(&vk_descriptor_buffer_infos[i]));
  }
}

#endif /* UNIFORMBUFFER_CPP */
