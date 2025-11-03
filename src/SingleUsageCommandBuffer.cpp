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

#include <VulkanEngine/Device.h>
#include <VulkanEngine/SingleUsageCommandBuffer.h>
#include <VulkanEngine/VulkanManager.h>

#include <limits>

void VulkanEngine::SingleUsageCommandBuffer::beginSingleUsageCommandBuffer() {
  auto command_buffer_info =
      vk::CommandBufferAllocateInfo()
          .setCommandBufferCount(1)
          .setCommandPool(
              VulkanManager::getInstance().getDevice()->getVkCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary);

  auto command_buffers = VulkanManager::getInstance()
                             .getDevice()
                             ->getVkDevice()
                             .allocateCommandBuffers(command_buffer_info);
  if (command_buffers.empty()) {
    throw std::runtime_error(
        "Could not allocate command buffer for buffer transfer!");
  }

  single_use_command_buffer = command_buffers[0];

  auto command_buffer_begin_info = vk::CommandBufferBeginInfo().setFlags(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  single_use_command_buffer.begin(command_buffer_begin_info);
}

void VulkanEngine::SingleUsageCommandBuffer::endSingleUsageCommandBuffer() {
  single_use_command_buffer.end();

  vk::FenceCreateInfo fenceInfo;
  vk::Fence fence =
      VulkanManager::getInstance().getDevice()->getVkDevice().createFence(
          fenceInfo);

  auto submit_info =
      vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(
          &single_use_command_buffer);

  VulkanManager::getInstance().getDevice()->getVkGraphicsQueue().submit(
      submit_info, fence);
  auto fence_result =
      VulkanManager::getInstance().getDevice()->getVkDevice().waitForFences(
          fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
  if (fence_result != vk::Result::eSuccess) {
    throw std::runtime_error("Error waiting for fences");
  }
  auto vk_device = VulkanManager::getInstance().getDevice()->getVkDevice();
  vk_device.destroyFence(fence);
  vk_device.freeCommandBuffers(
      VulkanManager::getInstance().getDevice()->getVkCommandPool(),
      single_use_command_buffer);
}
