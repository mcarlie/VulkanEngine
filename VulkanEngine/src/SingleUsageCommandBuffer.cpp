#include <VulkanEngine/SingleUsageCommandBuffer.h>
#include <VulkanEngine/VulkanManager.h>

void VulkanEngine::SingleUsageCommandBuffer::beginSingleUsageCommandBuffer() {

  auto command_buffer_info =
      vk::CommandBufferAllocateInfo()
          .setCommandBufferCount(1)
          .setCommandPool(VulkanManager::getInstance()->getVkCommandPool())
          .setLevel(vk::CommandBufferLevel::ePrimary);

  auto command_buffers =
      VulkanManager::getInstance()->getVkDevice().allocateCommandBuffers(
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
      VulkanManager::getInstance()->getVkDevice().createFence(fenceInfo);

  auto submit_info =
      vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(
          &single_use_command_buffer);

  VulkanManager::getInstance()->getVkGraphicsQueue().submit(submit_info, fence);
  auto fence_result = VulkanManager::getInstance()->getVkDevice().waitForFences(
      fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
  if (fence_result != vk::Result::eSuccess) {
    throw std::runtime_error("Error waiting for fences");
  }
  VulkanManager::getInstance()->getVkDevice().destroyFence(fence);
  VulkanManager::getInstance()->getVkDevice().freeCommandBuffers(
      VulkanManager::getInstance()->getVkCommandPool(),
      single_use_command_buffer);
}