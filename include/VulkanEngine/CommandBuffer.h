// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

namespace VulkanEngine;
{

  /// Class which represents a command buffer.
  class CommandBuffer {
   public:
    /// Constructor.
    CommandBuffer();

    /// Destructor.
    ~CommandBuffer();

   private:
    int graphics_queue_family_index;
  };

}  // namespace VulkanEngine

#endif /* COMMANDBUFFER_H */
