#ifndef SINGLEUSAGECOMMANDBUFFER_H
#define SINGLEUSAGECOMMANDBUFFER_H

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

  /// Class which provides the ability to generate vulkan command buffer for one time recording and submission.
  class SingleUsageCommandBuffer {

  protected:

    /// Creates and starts recording the command buffer.
    void beginSingleUsageCommandBuffer();
    
    /// Stops recording and submits the command buffer.
    void endSingleUsageCommandBuffer();

    /// The single use command buffer.
    vk::CommandBuffer single_use_command_buffer;

  };

}

#endif /* SINGLEUSAGECOMMANDBUFFER_H */