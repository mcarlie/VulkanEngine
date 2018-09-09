#ifndef SHADERMODULE_H
#define SHADERMODULE_H

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace VulkanTest {

  /// Class which represents a module in the shader pipeline, e.g vertex or fragment shader
  class ShaderModule {
  
  public:

    /// Constructor.
    /// \param file_path The full path to the file containing the source code of the shader
    /// \param shader_stage_flag The vk::ShaderStageFlagBits which indicates the type of shader stage
    ShaderModule( const std::string& file_path, vk::ShaderStageFlagBits shader_stage_flag );

    /// Destructor.
    ~ShaderModule();

    /// \return The vk::ShaderStageFlagBits flag for the ShaderModule
    const vk::ShaderStageFlagBits getVkShaderStageFlag();

    /// \return The internal vk::ShaderModule instance
    const vk::ShaderModule& getVkShaderModule();

  private:

    /// Read the source code file
    /// \param file_path The path to the file containing the source
    /// \param [out] Contains the bytes read from the file
    void readSource( const std::string& file_path, std::vector< char >& bytecode );

    /// The internal vk::ShaderModule instance
    vk::ShaderModule vk_shader_module;

    /// THe vk::ShaderStageFlagBits provided in the constructor
    vk::ShaderStageFlagBits vk_shader_stage_flag;

  };

}

#endif /* SHADERMODULE_H */