#include <VulkanTest/Shader.h>

VulkanTest::Shader::Shader( const std::vector< std::shared_ptr< ShaderModule > >& shader_modules ) {

  for( auto sm : shader_modules ) {
    auto shader_stage_info = vk::PipelineShaderStageCreateInfo()
      .setModule( sm->getVKShaderModule() )
      .setStage( sm->getShaderStageFlag() )
      .setPName( "main" );
    shader_stages.push_back( shader_stage_info );
  }

}

VulkanTest::Shader::~Shader() {

}

const std::vector< vk::PipelineShaderStageCreateInfo >& VulkanTest::Shader::getShaderStages() {
  return shader_stages;
}