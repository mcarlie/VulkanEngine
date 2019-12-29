#ifndef SHADERMODULE_H
#define SHADERMODULE_H

#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>
#include <filesystem>

namespace VulkanEngine {

  /// Class which represents a module in the shader pipeline, e.g vertex or fragment shader.
  class ShaderModule {
  
  public:

    /// Constructor.
    /// \param shader_string Either the source path or raw shader string
    /// \param shader_stage_flag The vk::ShaderStageFlagBits which indicates the type of shader stage.
    ShaderModule( const std::string& shader_string, bool is_filepath, vk::ShaderStageFlagBits shader_stage_flag );

    /// Destructor.
    ~ShaderModule();

    /// \return The vk::ShaderStageFlagBits flag for the ShaderModule.
    const vk::ShaderStageFlagBits getVkShaderStageFlag();

    /// \return The internal vk::ShaderModule instance.
    const vk::ShaderModule& getVkShaderModule();

  private:

    /// Read the source code file.
    /// \param file_path The path to the file containing the source.
    /// \param [out] Contains the bytes read from the file.
    void readSource( std::filesystem::path file_path, std::vector< uint32_t >& bytecode );
    
    /// Convert a GLSL shader string to SPIRV bytecode
    /// \param shader_string The GLSL shader string
    /// \param [out] spirv The resulting SPIRV bytecode
    void glslToSPIRV( const std::string& name, const std::string& shader_string, std::vector< uint32_t >& bytecode );

    /// The internal vk::ShaderModule instance.
    vk::ShaderModule vk_shader_module;

    /// The vk::ShaderStageFlagBits provided in the constructor.
    vk::ShaderStageFlagBits vk_shader_stage_flag;
    
    static bool glslang_initialized;

  };

}

#endif /* SHADERMODULE_H */
