#ifndef STAGEDBUFFER_H
#define STAGEDBUFFER_H

#include <VulkanEngine/Buffer.h>

namespace VulkanEngine {

/// Represents a temporary buffer used to transfer data to another
/// StagedBufferDestination buffer. This class can be templated on any class
/// inheriting from StagedBufferDestination in order to provide staging
/// functionality for that class. E.g the type StagedBuffer< Image > is an Image
/// which has staging functionality. \tparam DestinationClass The class which
/// will be the receiver of the data in this buffer. \tparam
/// DestinationClassArgs Parameter pack specifying the signature of the
/// constructor of DestinationClass
///                              to call from StagedBuffer's constructor.
template <class DestinationClass> class StagedBuffer : public DestinationClass {
public:
  /// Contructor.
  /// Creates the staging buffer.
  template <class... DestinationClassArgs>
  StagedBuffer(DestinationClassArgs... args);

  /// Destructor
  ~StagedBuffer();

  /// When called, data will be transferred from this staging buffer to the
  /// destination buffer. \param command_buffer The command buffer to record the
  /// transfer command in.
  ///                       If not specified an internal command buffer will be
  ///                       created and submitted to the appropriate queue.
  void transferBuffer(const vk::CommandBuffer &command_buffer = nullptr);

  /// Copy the data to the buffer.
  /// Overridden to update the source buffer.
  /// \param _data Pointer to the data.
  /// \param _data_size The size of the data in bytes.
  virtual void updateBuffer(const void *_data, size_t _data_size) override;

protected:
  /// The source buffer. Data will be transferred from this buffer to the
  /// destination buffer when calling transferBuffer().
  Buffer source_buffer;
};

} // namespace VulkanEngine

#include <StagedBuffer.cpp>

#endif /* STAGEDBUFFER_H */