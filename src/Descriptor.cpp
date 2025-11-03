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

#include <VulkanEngine/Descriptor.h>

VulkanEngine::Descriptor::Descriptor(
    uint32_t _binding, uint32_t _desciptor_count,
    vk::DescriptorType _vk_descriptor_type,
    vk::ShaderStageFlags _vk_shader_stage_flags)
    : binding(_binding),
      descriptor_count(_desciptor_count),
      vk_descriptor_type(_vk_descriptor_type),
      vk_shader_stage_flags(_vk_shader_stage_flags) {}

VulkanEngine::Descriptor::~Descriptor() {}

const vk::DescriptorSetLayoutBinding
VulkanEngine::Descriptor::getVkDescriptorSetLayoutBinding() const {
  return vk::DescriptorSetLayoutBinding()
      .setBinding(binding)
      .setDescriptorCount(descriptor_count)
      .setDescriptorType(vk_descriptor_type)
      .setStageFlags(vk_shader_stage_flags);
}

const vk::DescriptorPoolSize VulkanEngine::Descriptor::getVkDescriptorPoolSize()
    const {
  return vk::DescriptorPoolSize()
      .setDescriptorCount(descriptor_count)
      .setType(vk_descriptor_type);
}
