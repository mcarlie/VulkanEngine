#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

namespace VulkanEngine {

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

} // namespace VulkanEngine

#endif /* COMMANDBUFFER_H */
