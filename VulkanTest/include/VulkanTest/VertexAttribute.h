#ifndef VERTEXATTRIBUTE_H
#define VERTEXATTRIBUTE_H

#include <vulkan/vulkan.hpp>

#include <Eigen/Eigen>

#include <vector>

namespace VulkanTest {

  /// Class which represents a vertex attribute
  /// \tparam T The scalar type to use
  /// \tparam elements_x The number of elements in dimension x
  /// \tparam elements_y the number of elements in dimension y
  template< typename T, int elements_x, int elements_y >
  class VertexAttribute {

  public:

    VertexAttribute( const std::vector< Eigen::Matrix< T, elements_x, elements_y > >& _data ) : data( _data ) {
    }

    ~VertexAttribute() {
    };

    const vk::VertexInputBindingDescription getVkVertexInputBindingDescription() {

      return vk::VertexInputBindingDescription()
        .setBinding( 0 )
        .setInputRate( vk::VertexInputRate::eVertex )
        .setStride( sizeof( Eigen::Matrix< T, elements_x, elements_y > ) );

    }

  private:

    std::vector< Eigen::Matrix< T, elements_x, elements_y > > data;
  
  };

}

#endif /* VERTEXATTRIBUTE_H */