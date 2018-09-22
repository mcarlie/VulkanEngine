#ifndef MESHBASE_H
#define MESHBASE_H

#include <VulkanTest/Renderable.h>

namespace VulkanTest {

  /// Provides an abstract interface for various types of Mesh classes
  class MeshBase : public Renderable {
  
  public:

    /// Constructor
    MeshBase();

    /// Contructor which allows specifying which shader to render the Mesh with
    MeshBase( const std::shared_ptr< Shader > _shader );

    /// Destructor
    ~MeshBase();

    /// \return The vk::PipelineVertexInputStateCreateInfo instance describing the attributes that consitute the Mesh
    virtual const vk::PipelineVertexInputStateCreateInfo getVkPipelineVertexInputStateCreateInfo() = 0;

    /// Start transfer of data belonging to all associated VertexAttribute instances
    /// from staging buffer to vertex buffer memory
    /// \param command_buffer The vk::CommandBuffer to insert the commands into. 
    ///                       If none is provided a vk::CommandBuffer 
    ///                       will be allocated and executed automatically.
    virtual void transferBuffers( const vk::CommandBuffer& command_buffer = nullptr ) = 0;

    /// Bind VertexBuffers in this Mesh which will be used for rendering
    /// \param command_buffer The vk::CommandBuffer to insert the commands into
    virtual void bindVertexBuffers( const vk::CommandBuffer& command_buffer ) = 0;

    /// Bind the index buffer of this Mesh
    /// \param command_buffer The vk::CommandBuffer to insert the commands into
    virtual void bindIndexBuffer( const vk::CommandBuffer& command_buffer ) = 0;

    /// Insert drawing commands
    /// \param command_buffer The vk::CommandBuffer to insert the commands into
    virtual void draw( const vk::CommandBuffer& command_buffer ) = 0;

  };

}

#endif /* MESHBASE_H */