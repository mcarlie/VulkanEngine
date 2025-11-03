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

#include <VulkanEngine/Buffer.h>
#include <VulkanEngine/VulkanManager.h>

VulkanEngine::Buffer::Buffer(size_t _data_size,
                             vk::BufferUsageFlags usage_flags,
                             vk::MemoryPropertyFlags memory_property_flags,
                             VmaMemoryUsage vma_memory_usage)
    : data_size(_data_size) {
  createBuffer(usage_flags, memory_property_flags, vma_memory_usage);
}

VulkanEngine::Buffer::~Buffer() {
  vmaDestroyBuffer(VulkanManager::getInstance().getDevice()->getVmaAllocator(),
                   static_cast<VkBuffer>(vk_buffer), vma_allocation);
}

const vk::Buffer VulkanEngine::Buffer::getVkBuffer() const { return vk_buffer; }

void VulkanEngine::Buffer::createBuffer(
    vk::BufferUsageFlags usage_flags,
    vk::MemoryPropertyFlags memory_property_flags,
    VmaMemoryUsage vma_memory_usage) {
  auto& vulkan_manager = VulkanManager::getInstance();

  auto buffer_info = vk::BufferCreateInfo()
                         .setSize(data_size)
                         .setUsage(usage_flags)
                         .setSharingMode(vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo vma_allocation_create_info = {};
  vma_allocation_create_info.usage = vma_memory_usage;
  vma_allocation_create_info.requiredFlags =
      static_cast<VkMemoryPropertyFlags>(memory_property_flags);

  vk::Buffer buffer;
  VkBufferCreateInfo buffer_create_info_c_handle =
      static_cast<VkBufferCreateInfo>(buffer_info);

  auto result = vmaCreateBuffer(
      vulkan_manager.getDevice()->getVmaAllocator(),
      &buffer_create_info_c_handle, &vma_allocation_create_info,
      reinterpret_cast<VkBuffer*>(&buffer), &vma_allocation, nullptr);

  if (result != VK_SUCCESS) {
    throw std::runtime_error("Could not create buffer!");
  }

  vk_buffer = buffer;
}

void VulkanEngine::Buffer::insertTransferCommand(
    const vk::CommandBuffer& command_buffer, const vk::Buffer& source_buffer) {
  auto buffer_copy =
      vk::BufferCopy().setSrcOffset(0).setDstOffset(0).setSize(data_size);

  command_buffer.copyBuffer(source_buffer, vk_buffer, buffer_copy);
}

size_t VulkanEngine::Buffer::getStagingBufferSize() const { return data_size; }
