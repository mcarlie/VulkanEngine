#ifndef MESHBASE_H
#define MESHBASE_H

#include <VulkanEngine/BoundingBox.h>
#include <VulkanEngine/Shader.h>

#include <memory>

namespace VulkanEngine {

/// Base class for Mesh specializations.
class MeshBase {
public:
  /// Constructor.
  MeshBase();

  /// Destructor.
  ~MeshBase();

  /// \return The vk::PipelineVertexInputStateCreateInfo instance describing the
  /// attributes that consitute the Mesh.
  virtual const vk::PipelineVertexInputStateCreateInfo &
  getVkPipelineVertexInputStateCreateInfo() = 0;

  /// \return The vk::PipelineInputAssemblyStateCreateInfo describing how to
  /// handle the input assembly stage for the Mesh.
  virtual const vk::PipelineInputAssemblyStateCreateInfo &
  getVkPipelineInputAssemblyStateCreateInfo() = 0;

  /// Start transfer of data belonging to all associated VertexAttribute
  /// instances from staging buffer to vertex buffer memory. \param
  /// command_buffer The vk::CommandBuffer to insert the commands into.
  ///                       If none is provided a vk::CommandBuffer
  ///                       will be allocated and executed automatically.
  virtual void
  transferBuffers(const vk::CommandBuffer &command_buffer = nullptr) = 0;

  /// Bind VertexBuffers in this Mesh which will be used for rendering.
  /// \param command_buffer The vk::CommandBuffer to insert the commands into.
  virtual void bindVertexBuffers(const vk::CommandBuffer &command_buffer) = 0;

  /// Bind the index buffer of this Mesh.
  /// \param command_buffer The vk::CommandBuffer to insert the commands into.
  virtual void bindIndexBuffer(const vk::CommandBuffer &command_buffer) = 0;

  /// Insert drawing commands.
  /// \param command_buffer The vk::CommandBuffer to insert the commands into.
  virtual void draw(const vk::CommandBuffer &command_buffer) = 0;

  template <typename PositionType>
  const BoundingBox<PositionType> &getBoundingBox() {
    return *static_cast<const BoundingBox<PositionType> *>(bounding_box.get());
  }

protected:

  /// The Mesh's bounding box.
  std::shared_ptr<BoundingBoxBase> bounding_box;
};

} // namespace VulkanEngine

#endif /* MESHBASE_H */
