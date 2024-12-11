// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_MESH_H_
#define INCLUDE_VULKANENGINE_MESH_H_

#include <VulkanEngine/IndexAttribute.h>
#include <VulkanEngine/MeshBase.h>
#include <VulkanEngine/VertexAttribute.h>

#include <Eigen/Eigen>
#include <memory>
#include <tuple>
#include <vector>

namespace VulkanEngine {

/// Represents a mesh consisting of VertexAttribute instances.
/// The mesh will at the very least support positions and indices. Any
/// additional types that should be supported can be specified as template
/// arguments. Example: Mesh< Eigen::Vector3f, uint32_t, Eigen::Vector3f,
/// Eigen::Vector3f > represents a mesh which supports Eigen::Vector3f
/// positions, uint32_t indices as well as additional Eigen::Vector3f and
/// Eigen::Vector2f vertex attributes which could for example represent normals
/// and texture coordinates. \tparam PositionType The scalar type to use for the
/// positions, e.g float, double. \tparam IndexType The type to use for storing
/// indices, e.g uint16_t or uint32_t. \tparam AdditionalAttributeTypes A
/// variadic list of additional VertexAttribute types supported by the mesh.
template <
    typename PositionType,              // NOLINT(whitespace/indent_namespace)
    typename IndexType,                 // NOLINT(whitespace/indent_namespace)
    class... AdditionalAttributeTypes>  // NOLINT(whitespace/indent_namespace)
class Mesh : public MeshBase {
  static_assert(sizeof(IndexType) == sizeof(uint16_t) ||
                    sizeof(IndexType) == sizeof(uint32_t),
                "Mesh IndexType template parameter must be the same size as "
                "either uint16_t or uint32_t");

 public:
  /// Defines the container for storing additional attributes.
  /// \tparam T The type of the VertexAttribute.
  template <typename T>
  using AttributeContainer = std::vector<std::shared_ptr<VertexAttribute<T>>>;

  /// Default constructor.
  Mesh();

  /// Constructor.
  /// \param _positions A VertexAttribute giving the positions of the vertices
  /// of the mesh. \param _indices An IndexAttribute giving the indices of the
  /// mesh. \param _attribute A list of additional VertexAttribute instances
  /// which will be used when rendering the mesh. \param _shader The shader to
  /// use when rendering the mesh.
  Mesh(const std::shared_ptr<VertexAttribute<PositionType>> &_positions,
       const std::shared_ptr<IndexAttribute<IndexType>> &_indices,
       const std::tuple<AttributeContainer<AdditionalAttributeTypes>...>
           &_attributes,
       const std::shared_ptr<Shader> &_shader);

  /// Destructor.
  virtual ~Mesh();

  /// Sets the positions of the Mesh.
  /// \param _positions A VertexAttribute giving the positions of the vertices
  /// of the Mesh.
  void setPositions(
      const std::shared_ptr<VertexAttribute<PositionType>> &_positions);

  /// Sets the indices of the Mesh.
  /// \param _indices An IndexAttribute giving the indices of the Mesh.
  void setIndices(const std::shared_ptr<IndexAttribute<IndexType>> &_indices);

  /// Sets the additional attributes of the Mesh.
  /// \param _additional_attributes A list of additional VertexAttribute
  /// instances which will be used when rendering the Mesh.
  void setAttributes(
      const std::tuple<AttributeContainer<AdditionalAttributeTypes>...>
          &_attributes);

  /// Set the Mesh's BoundingBox.
  /// \param max The maximum position of the bounding box.
  /// \param min The minimum position of the bounding box.
  void setBoundingBox(const PositionType &max, const PositionType &min);

  /// \return The vk::PipelineVertexInputStateCreateInfo instance describing the
  /// attributes that constitute the Mesh.
  virtual const vk::PipelineVertexInputStateCreateInfo &
  createVkPipelineVertexInputStateCreateInfo();

  /// \return The vk::PipelineInputAssemblyStateCreateInfo describing how to
  /// handle the input assembly stage for the Mesh.
  virtual const vk::PipelineInputAssemblyStateCreateInfo &
  createVkPipelineInputAssemblyStateCreateInfo();

  /// Start transfer of data belonging to all associated VertexAttribute
  /// instances from staging buffer to vertex buffer memory. \param
  /// command_buffer The vk::CommandBuffer to insert the commands into.
  ///                       If not specified an internal command buffer will be
  ///                       created and submitted to the graphics queue.
  virtual void transferBuffers(
      const vk::CommandBuffer &command_buffer = nullptr);

  /// Bind VertexBuffers in this Mesh which will be used for rendering.
  /// \param command_buffer The vk::CommandBuffer to insert the commands into.
  virtual void bindVertexBuffers(const vk::CommandBuffer &command_buffer);

  /// Bind the index buffer of this Mesh.
  /// \param command_buffer The vk::CommandBuffer to insert the commands into.
  virtual void bindIndexBuffer(const vk::CommandBuffer &command_buffer);

  /// Insert drawing commands.
  /// \param command_buffer The vk::CommandBuffer to insert the commands into.
  virtual void draw(const vk::CommandBuffer &command_buffer);

 private:
  /// The positions defining the Mesh.
  std::shared_ptr<VertexAttribute<PositionType>> positions;

  /// The indices of the Mesh.
  std::shared_ptr<IndexAttribute<IndexType>> indices;

  /// Any additional attributes such as color and so on.
  std::tuple<AttributeContainer<AdditionalAttributeTypes>...> attributes;

  /// vk::VertexInputBindingDescription for each attribute.
  std::vector<vk::VertexInputBindingDescription> binding_descriptions;

  /// vk::VertexInputAttributeDescription for each attribute.
  std::vector<vk::VertexInputAttributeDescription> attribute_descriptions;

  vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state_info;

  vk::PipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_info;

  bool pipeline_input_state_info_initialized;
};

}  // namespace VulkanEngine

#include <Mesh.cpp>  // NOLINT(build/include)

#endif  // INCLUDE_VULKANENGINE_MESH_H_
