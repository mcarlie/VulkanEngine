// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_COMMANDBUFFER_H_
#define INCLUDE_VULKANENGINE_COMMANDBUFFER_H_

namespace VulkanEngine {

/// Class which represents a command buffer.
class CommandBuffer {
 public:
  /// Destructor.
  ~CommandBuffer();

 private:
  int graphics_queue_family_index;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_COMMANDBUFFER_H_
