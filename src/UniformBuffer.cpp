#ifndef UNIFORMBUFFER_CPP
#define UNIFORMBUFFER_CPP

#include <VulkanEngine/UniformBuffer.h>

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

template <typename T> VulkanEngine::UniformBuffer<T>::~UniformBuffer() {}

template <typename T>
void VulkanEngine::UniformBuffer<T>::appendVkDescriptorSets(
    std::vector<vk::WriteDescriptorSet> &write_descriptor_sets,
    std::vector<vk::CopyDescriptorSet> &copy_descriptor_sets,
    const vk::DescriptorSet &destination_set) {
  vk_descriptor_buffer_infos.clear();

  for (uint32_t i = 0; i < array_size; ++i) {
    vk_descriptor_buffer_infos.push_back(
        vk::DescriptorBufferInfo()
            .setBuffer(getVkBuffer())
            .setOffset(sizeof(T) * static_cast<size_t>(i))
            .setRange(sizeof(T)));

    write_descriptor_sets.push_back(
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