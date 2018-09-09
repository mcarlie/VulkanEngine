#ifndef VERTEXATTRIBUTE_H
#define VERTEXATTRIBUTE_H

#include <VulkanTest/Attribute.h>

#include <vulkan/vulkan.hpp>

#include <Eigen/Eigen>

#include <vector>

namespace VulkanTest {

  /// Class which represents a vertex attribute
  /// \tparam T The scalar type to use
  /// \tparam elements_x The number of elements in dimension x
  /// \tparam elements_y the number of elements in dimension y
  template< typename T, int elements_x, int elements_y >
  class VertexAttribute : public Attribute {

  public:

    /// Constructor
    /// \param data The vertex data which will be represented by this VertexAttribute instance
    /// \param binding The binding index of the vertex attribute
    VertexAttribute( const std::vector< Eigen::Matrix< T, elements_x, elements_y > >& _data, uint32_t _binding );

    /// Destructor
    ~VertexAttribute();

    /// \return The Vulkan VertexInputBindingDescription for this VertexAttribute instance
    const vk::VertexInputBindingDescription getVkVertexInputBindingDescription();

  private:

    uint32_t binding;

  };

}

#include <VertexAttribute.cpp>

#endif /* VERTEXATTRIBUTE_H */