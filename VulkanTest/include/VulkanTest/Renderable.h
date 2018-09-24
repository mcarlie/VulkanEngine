#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <VulkanTest/Shader.h>

#include <vulkan/vulkan.hpp>

#include <string>

namespace VulkanTest {
  
  /// An abstract base class for any object which can be rendered using a shader
  class Renderable {

  public:

    /// Constructor
    Renderable();

    /// Constructor which allows specifying the shader to use
    /// \param _shader The shader to use
    Renderable( const std::shared_ptr< Shader >& _shader );

    /// Destructor
    ~Renderable();

    /// \return the Mesh's shader
    const std::shared_ptr< Shader > getShader();

    /// Sets the Mesh's shader
    /// \param _shader The shader to use when rendering the Mesh
    void setShader( const std::shared_ptr< Shader >& _shader );

  private:

    /// The shader to use when rendering this Renderable object
    std::shared_ptr< Shader > shader;

  };

}

#endif /* RENDERABLE_H */