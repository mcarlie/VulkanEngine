// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_SINGLEUSAGECOMMANDBUFFER_H_
#define INCLUDE_VULKANENGINE_SINGLEUSAGECOMMANDBUFFER_H_

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Class which provides the ability to generate vulkan command buffer for one
/// time recording and submission.
/// TODO Make this a specialization of a generic command buffer class
class SingleUsageCommandBuffer {
 public:
  /// Creates and starts recording the command buffer.
  void beginSingleUsageCommandBuffer();

  /// Stops recording and submits the command buffer.
  void endSingleUsageCommandBuffer();

  /// The single use command buffer.
  vk::CommandBuffer single_use_command_buffer;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_SINGLEUSAGECOMMANDBUFFER_H_
