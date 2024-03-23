#ifndef SHADER_H
#define SHADER_H

#include <VulkanEngine/Descriptor.h>
#include <VulkanEngine/ShaderModule.h>

namespace VulkanEngine {

/// Class which encapsulates a single shader consisting of several ShaderModule
/// instances.
class Shader {
public:
  /// Constructor.
  /// \param shader_modules ShaderModule instances to use in this shader.
  Shader(const std::vector<std::shared_ptr<ShaderModule>> &_shader_modules);

  /// Destructor.
  ~Shader();

  /// Set descriptors for this shader, such as images and buffers.
  /// \param _descriptors List of descriptors to set.
  void
  setDescriptors(const std::vector<std::vector<std::shared_ptr<Descriptor>>>
                     &_descriptors);

  /// Bind a set of descriptors to an index.
  /// \param command_buffer Command buffer used for binding.
  /// \param descriptor_set_index The index to bind to.
  void bindDescriptorSet(const vk::CommandBuffer &command_buffer,
                         uint32_t descriptor_set_index);

  /// \return Vulkan PipelineShaderStageCreateInfo instances.
  const std::vector<vk::PipelineShaderStageCreateInfo> &getVkShaderStages();

  /// \return Vulkan PipelineLayout instances.
  const vk::PipelineLayout getVkPipelineLayout();

private:
  /// List of ShaderStages for this shader.
  std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

  /// ShaderModule instances for this shader.
  std::vector<std::shared_ptr<ShaderModule>> shader_modules;

  /// All Descriptor objects used in this shader.
  std::vector<std::vector<std::shared_ptr<Descriptor>>> descriptors;

  /// Vulkan descriptor pool
  vk::DescriptorPool vk_descriptor_pool;

  /// Vulkan DescriptorSetLayout.
  std::vector<vk::DescriptorSetLayout> vk_descriptor_set_layouts;

  /// List of Vulkan DescriptorSets
  std::vector<vk::DescriptorSet> vk_descriptor_sets;

  /// Vulkan PipelineLayout.
  vk::PipelineLayout vk_pipeline_layout;
};

} // namespace VulkanEngine

#endif /* SHADER_H */
