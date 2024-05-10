#include <VulkanEngine/SingleUsageCommandBuffer.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Device.h>

void VulkanEngine::SingleUsageCommandBuffer::beginSingleUsageCommandBuffer() {
  auto command_buffer_info =
      vk::CommandBufferAllocateInfo()
          .setCommandBufferCount(1)
          .setCommandPool(VulkanManager::getInstance().getDevice()->getVkCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary);

  auto command_buffers =
      VulkanManager::getInstance().getDevice()->getVkDevice().allocateCommandBuffers(
          command_buffer_info);
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
      VulkanManager::getInstance().getDevice()->getVkDevice().createFence(fenceInfo);

  auto submit_info =
      vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(
          &single_use_command_buffer);

  VulkanManager::getInstance().getDevice()->getVkGraphicsQueue().submit(submit_info, fence);
  auto fence_result = VulkanManager::getInstance().getDevice()->getVkDevice().waitForFences(
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