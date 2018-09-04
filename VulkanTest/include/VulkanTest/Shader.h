#ifndef SHADER_H
#define SHADER_H

#include <VulkanTest/ShaderModule.h>

namespace VulkanTest {

  /// Class which encapsulates a single shader consisting of several ShaderModule instances
  class Shader {

  public:

    /// Constructor
    Shader( const std::vector< std::shared_ptr< ShaderModule > >& shader_modules );
    
    /// Destructor
    ~Shader();

    const std::vector< vk::PipelineShaderStageCreateInfo >& getShaderStages();

  private:

    std::vector< vk::PipelineShaderStageCreateInfo > shader_stages;

  };

}

#endif /* SHADER_H */