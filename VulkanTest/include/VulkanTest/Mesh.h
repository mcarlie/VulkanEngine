#ifndef MESH_H
#define MESH_H

#include <VulkanTest/Renderable.h>
#include <VulkanTest/VertexAttribute.h>
#include <VulkanTest/IndexAttribute.h>
#include <VulkanTest/Shader.h>

#include <Eigen/Eigen>

namespace VulkanTest {

  /// Represents a mesh consisting of VertexAttribute instances
  /// \tparam PositionType The scalar type to use for the positions, e.g float, double
  /// \tparam IndexType The type to use for storing indices, e.g uint16_t or uint32_t
  /// \tparam AdditionalAttributeTypes A variadic list of additional VertexAttribute types supported by the mesh
  template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
  class Mesh : public Renderable {

  public:

    /// Defines the container for storing additional attributes
    template< typename T >
    using AttributeContainer = std::vector< std::shared_ptr< VertexAttribute< T > > >;

    /// Default constructor
    Mesh();

    /// Constructor
    /// \param _positions A VertexAttribute giving the positions of the vertices of the mesh
    /// \param _indices An IndexAttribute giving the indices of the mesh
    /// \param _attribute A list of additional VertexAttribute instances which will be used when rendering the mesh
    /// \param _shader The shader to use when rendering the mesh
    Mesh( 
      const std::shared_ptr< VertexAttribute< Eigen::Matrix< PositionType, 3, 1 > > >& _positions,
      const std::shared_ptr< IndexAttribute< IndexType > >& _indices,
      const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes,
      const std::shared_ptr< Shader >& _shader
      );
    
    /// Destructor
    ~Mesh();

  protected:

    /// Sets the positions of the Mesh
    /// \param _positions A VertexAttribute giving the positions of the vertices of the Mesh
    void setPositions( const std::shared_ptr< VertexAttribute< Eigen::Matrix< PositionType, 3, 1 > > >& _positions );
    
    /// Sets the indices of the Mesh
    /// \param _indices An IndexAttribute giving the indices of the Mesh
    void setIndices( const std::shared_ptr< IndexAttribute< IndexType > >& _indices );
    
    /// Sets the additional attributes of the Mesh
    /// \param _attribute A list of additional VertexAttribute instances which will be used when rendering the Mesh
    void setAttributes( const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes );

    /// Sets the Mesh's shader
    /// \param _shader The shader to use when rendering the Mesh
    void setShader( const std::shared_ptr< Shader >& _shader );

  private:

    /// The positions defining the Mesh
    std::shared_ptr< VertexAttribute< Eigen::Matrix< PositionType, 3, 1 > > > positions;
    
    /// The indices of the Mesh
    std::shared_ptr< IndexAttribute< IndexType > > indices;

    /// Any additional attributes such as color and so on
    std::tuple< AttributeContainer< AdditionalAttributeTypes > ... > attributes;

    /// The shader to use when rendering this Mesh
    std::shared_ptr< Shader > shader;

  };

}

#include <Mesh.cpp>

#endif /* MESH_H */