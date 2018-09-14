#ifndef VERTEXATTRIBUTE_H
#define VERTEXATTRIBUTE_H

#include <VulkanTest/Attribute.h>

#include <vulkan/vulkan.hpp>

#include <vector>

namespace VulkanTest {

  /// Class which represents a vertex attribute
  /// \tparam T The type to use to represent data elements
  template< typename T >
  class VertexAttribute : public Attribute {

  public:

    /// Constructor
    /// \param data The vertex data which will be represented by this VertexAttribute instance
    /// \param binding The binding index of the vertex attribute
    VertexAttribute( const std::vector< T >& data, uint32_t _binding );

    /// Destructor
    ~VertexAttribute();

    /// \return The Vulkan VertexInputBindingDescription for this VertexAttribute instance
    const vk::VertexInputBindingDescription getVkVertexInputBindingDescription();

  private:

    /// The binding index of the vertex buffer
    uint32_t binding;

  };

}

#include <VertexAttribute.cpp>

#endif /* VERTEXATTRIBUTE_H */