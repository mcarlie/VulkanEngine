#ifndef MESH_H
#define MESH_H

#include <VulkanTest/MeshBase.h>
#include <VulkanTest/VertexAttribute.h>
#include <VulkanTest/IndexAttribute.h>

#include <Eigen/Eigen>

namespace VulkanTest {

  /// Represents a mesh consisting of VertexAttribute instances
  /// The mesh will at the very least support 3D positions and indices. Any additional
  /// types that should be supported can be specified as template arguments.
  /// Example: Mesh< float, uint32_t, Eigen::Vector3f, Eigen::Vector3f > represents a mesh which supports
  /// Eigen::Matrix< float, 3, 1 > positions, uint32_t indices as well as additional Eigen::Vector3f and Eigen::Vector2f
  /// vertex attributes which could for example represent normals and texture coordinates
  /// \tparam PositionType The scalar type to use for the positions, e.g float, double
  /// \tparam IndexType The type to use for storing indices, e.g uint16_t or uint32_t
  /// \tparam AdditionalAttributeTypes A variadic list of additional VertexAttribute types supported by the mesh
  template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
  class Mesh : public MeshBase {

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

    /// Sets the positions of the Mesh
    /// \param _positions A VertexAttribute giving the positions of the vertices of the Mesh
    void setPositions( const std::shared_ptr< VertexAttribute< Eigen::Matrix< PositionType, 3, 1 > > >& _positions );
    
    /// Sets the indices of the Mesh
    /// \param _indices An IndexAttribute giving the indices of the Mesh
    void setIndices( const std::shared_ptr< IndexAttribute< IndexType > >& _indices );
    
    /// Sets the additional attributes of the Mesh
    /// \param _attribute A list of additional VertexAttribute instances which will be used when rendering the Mesh
    void setAttributes( const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes );

    /// \return The vk::PipelineVertexInputStateCreateInfo instance describing the attributes that constitute the Mesh
    virtual const vk::PipelineVertexInputStateCreateInfo getVkPipelineVertexInputStateCreateInfo();

    /// Start transfer of data belonging all associated VertexAttribute instances
    /// from staging buffer to vertex buffer memory
    /// \param command_buffer The vk::CommandBuffer to insert the commands into
    virtual void transferBuffers( const vk::CommandBuffer& command_buffer = nullptr );

    /// Bind VertexBuffers in this Mesh which will be used for rendering
    /// \param command_buffer The vk::CommandBuffer to insert the commands into
    virtual void bindVertexBuffers( const vk::CommandBuffer& command_buffer );

    /// Bind the index buffer of this Mesh
    /// \param command_buffer The vk::CommandBuffer to insert the commands into
    virtual void bindIndexBuffer( const vk::CommandBuffer& command_buffer );

    /// Insert drawing commands
    /// \param command_buffer The vk::CommandBuffer to insert the commands into
    virtual void draw( const vk::CommandBuffer& command_buffer );

  private:

    /// The positions defining the Mesh
    std::shared_ptr< VertexAttribute< Eigen::Matrix< PositionType, 3, 1 > > > positions;
    
    /// The indices of the Mesh
    std::shared_ptr< IndexAttribute< IndexType > > indices;

    /// Any additional attributes such as color and so on
    std::tuple< AttributeContainer< AdditionalAttributeTypes > ... > additional_attributes;

    /// vk::VertexInputBindingDescription for each attribute
    std::vector< vk::VertexInputBindingDescription > binding_descriptions;
    
    /// vk::VertexInputAttributeDescription for each attribute
    std::vector< vk::VertexInputAttributeDescription > attribute_descriptions;

  };

}

#include <Mesh.cpp>

#endif /* MESH_H */