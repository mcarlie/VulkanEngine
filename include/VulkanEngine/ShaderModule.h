// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_SHADERMODULE_H_
#define INCLUDE_VULKANENGINE_SHADERMODULE_H_

#include <filesystem>  // NOLINT(build/c++17)
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Class which represents a module in the shader pipeline, e.g vertex or
/// fragment shader.
class ShaderModule {
 public:
  /// Constructor.
  /// \param shader_string Either the source path or raw shader string
  /// \param shader_stage_flag The vk::ShaderStageFlagBits which indicates the
  /// type of shader stage.
  ShaderModule(const std::string& shader_string, bool is_filepath,
               vk::ShaderStageFlagBits shader_stage_flag);

  /// Destructor.
  ~ShaderModule();

  /// \return The vk::ShaderStageFlagBits flag for the ShaderModule.
  const vk::ShaderStageFlagBits getVkShaderStageFlag() const;

  /// \return The internal vk::ShaderModule instance.
  const vk::ShaderModule& getVkShaderModule() const;

 private:
  /// Read the source code file.
  /// \param file_path The path to the file containing the source.
  /// \returns The bytes read from the file.
  std::vector<uint32_t> readSource(std::filesystem::path file_path);

  /// Convert a GLSL shader string to SPIRV bytecode
  /// \param shader_string The GLSL shader string
  /// \param [out] spirv The resulting SPIRV bytecode
  std::vector<uint32_t> glslToSPIRV(const std::string& name,
                                    const std::string& shader_string);

  /// The internal vk::ShaderModule instance.
  vk::ShaderModule vk_shader_module;

  /// The vk::ShaderStageFlagBits provided in the constructor.
  vk::ShaderStageFlagBits vk_shader_stage_flag;

  static bool glslang_initialized;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_SHADERMODULE_H_
