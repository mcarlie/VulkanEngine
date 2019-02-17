#ifndef SHADER_H
#define SHADER_H

#include <VulkanEngine/ShaderModule.h>
#include <VulkanEngine/Descriptor.h>

namespace VulkanEngine {

  /// Class which encapsulates a single shader consisting of several ShaderModule instances.
  class Shader {

  public:
    
    /// Constructor.
    /// \param shader_modules ShaderModule instances to use in this shader.
    Shader( const std::vector< std::shared_ptr< ShaderModule > >& _shader_modules );
    
    /// Destructor.
    ~Shader();

    void setDescriptors( const std::vector< std::vector< std::shared_ptr< Descriptor > > >& _descriptors );

    void bindDescriptorSet( const vk::CommandBuffer& command_buffer, uint32_t descriptor_set_index );

    const std::vector< vk::PipelineShaderStageCreateInfo >& getVkShaderStages();

    const vk::PipelineLayout getVkPipelineLayout();

  private:

    std::vector< vk::PipelineShaderStageCreateInfo > shader_stages;

	  std::vector< std::shared_ptr< ShaderModule > > shader_modules;

    /// All Descriptor objects used in this shader.
    std::vector< std::vector< std::shared_ptr< Descriptor > > > descriptors;

    vk::DescriptorPool vk_descriptor_pool;

    std::vector< vk::DescriptorSetLayout > vk_descriptor_set_layouts;

    std::vector< vk::DescriptorSet > vk_descriptor_sets;

    vk::PipelineLayout vk_pipeline_layout;

  };

}

#endif /* SHADER_H */
