#ifndef VERTEXATTRIBUTE_H
#define VERTEXATTRIBUTE_H

#include <VulkanEngine/Attribute.h>

#include <vector>
#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Class which represents a vertex attribute.
/// \tparam T The type to use to represent data elements.
template <typename T> class VertexAttribute : public Attribute {
public:
  /// Constructor.
  /// \param data The vertex data which will be represented by this
  /// VertexAttribute instance. \param binding The binding index of the vertex
  /// attribute.
  VertexAttribute(const T *data, size_t _num_elements, uint32_t _location,
                  vk::Format _format);

  /// Destructor.
  virtual ~VertexAttribute();

  /// \return The Vulkan VertexInputBindingDescription for this VertexAttribute
  /// instance.
  const vk::VertexInputBindingDescription
  getVkVertexInputBindingDescription(uint32_t binding_index) const;

  /// \return All vk::VertexInputAttributeDescription for this VertexAttribute
  /// instance.
  const vk::VertexInputAttributeDescription
  getVkVertexInputAttributeDescriptions(uint32_t binding_index) const;

  /// \return The vk::Format of the VertexAttribute describing the format of the
  /// vertex data.
  vk::Format getVkFormat() const;

private:
  /// Number representing the shader location of the VertexAttribute.
  uint32_t location;

  /// The format of the attribute's data.
  vk::Format format;
};

} // namespace VulkanEngine

#include <VertexAttribute.cpp>

#endif /* VERTEXATTRIBUTE_H */
