#include <VulkanEngine/Shader.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Device.h>

VulkanEngine::Shader::Shader(
    const std::vector<std::shared_ptr<ShaderModule>> &_shader_modules) {
  shader_modules = _shader_modules;
  for (const auto &sm : shader_modules) {
    auto shader_stage_info = vk::PipelineShaderStageCreateInfo()
                                 .setModule(sm->getVkShaderModule())
                                 .setStage(sm->getVkShaderStageFlag())
                                 .setPName("main");
    shader_stages.push_back(shader_stage_info);
  }
}

VulkanEngine::Shader::~Shader() {
  const auto &vk_device = VulkanManager::getInstance().getDevice()->getVkDevice();
  vk_device.destroyDescriptorPool(vk_descriptor_pool);
  vk_device.destroyPipelineLayout(vk_pipeline_layout);
  for (const auto &dsl : vk_descriptor_set_layouts) {
    vk_device.destroyDescriptorSetLayout(dsl);
  }
}

void VulkanEngine::Shader::setDescriptors(
    const std::vector<std::vector<std::shared_ptr<Descriptor>>> &_descriptors) {
  const auto &vk_device = VulkanManager::getInstance().getDevice()->getVkDevice();

  // Causes the pipeline layout to be recreated and include changes to
  // descriptors
  if (vk_pipeline_layout) {
    vk_device.destroyPipelineLayout(vk_pipeline_layout);
  }

  descriptors = _descriptors;

  std::vector<vk::DescriptorPoolSize> pool_sizes;
  std::vector<std::vector<vk::DescriptorSetLayoutBinding>>
      descriptor_set_layout_bindings;

  for (const auto &d_vec : descriptors) {
    descriptor_set_layout_bindings.push_back(
        std::vector<vk::DescriptorSetLayoutBinding>());
    for (const auto &d : d_vec) {
      pool_sizes.push_back(d->getVkDescriptorPoolSize());
      descriptor_set_layout_bindings.back().push_back(
          d->getVkDescriptorSetLayoutBinding());
    }
  }

  auto pool_create_info =
      vk::DescriptorPoolCreateInfo()
          .setPoolSizeCount(static_cast<uint32_t>(pool_sizes.size()))
          .setPPoolSizes(pool_sizes.data())
          .setMaxSets(static_cast<uint32_t>(descriptors.size()));

  if (vk_descriptor_pool) {
    vk_device.destroyDescriptorPool(vk_descriptor_pool);
  }
  vk_descriptor_pool = vk_device.createDescriptorPool(pool_create_info);

  for (uint32_t i = 0; i < descriptors.size(); ++i) {
    auto descriptor_set_layout_info =
        vk::DescriptorSetLayoutCreateInfo()
            .setBindingCount(
                static_cast<uint32_t>(descriptor_set_layout_bindings[i].size()))
            .setPBindings(descriptor_set_layout_bindings[i].data());

    vk_descriptor_set_layouts.push_back(
        vk_device.createDescriptorSetLayout(descriptor_set_layout_info));
  }

  auto descriptor_set_allocate_info =
      vk::DescriptorSetAllocateInfo()
          .setDescriptorPool(vk_descriptor_pool)
          .setDescriptorSetCount(static_cast<uint32_t>(descriptors.size()))
          .setPSetLayouts(vk_descriptor_set_layouts.data());

  vk_descriptor_sets =
      vk_device.allocateDescriptorSets(descriptor_set_allocate_info);

  for (size_t i = 0; i < descriptors.size(); ++i) {
    auto write_descriptor_sets = std::make_shared<std::vector<vk::WriteDescriptorSet>>();
    auto copy_descriptor_sets = std::make_shared<std::vector<vk::CopyDescriptorSet>>();

    for (const auto &d : descriptors[i]) {
      d->appendVkDescriptorSets(write_descriptor_sets, copy_descriptor_sets,
                                vk_descriptor_sets[i]);
    }

    vk_device.updateDescriptorSets(*write_descriptor_sets.get(), nullptr);
  }
}

void VulkanEngine::Shader::bindDescriptorSet(
    const vk::CommandBuffer &command_buffer, uint32_t descriptor_set_index) {
  if (descriptor_set_index < vk_descriptor_sets.size()) {
    command_buffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, createVkPipelineLayout(), 0,
        vk_descriptor_sets[descriptor_set_index], nullptr);
  }
}

const std::vector<vk::PipelineShaderStageCreateInfo> &
VulkanEngine::Shader::getVkShaderStages() const {
  return shader_stages;
}

const vk::PipelineLayout VulkanEngine::Shader::createVkPipelineLayout() {
  if (!vk_pipeline_layout) {
    auto pipeline_layout_info =
        vk::PipelineLayoutCreateInfo()
            .setSetLayoutCount(
                static_cast<uint32_t>(vk_descriptor_set_layouts.size()))
            .setPSetLayouts(vk_descriptor_set_layouts.data())
            .setPushConstantRangeCount(0)
            .setPPushConstantRanges(nullptr);
    vk_pipeline_layout =
        VulkanManager::getInstance().getDevice()->getVkDevice().createPipelineLayout(
            pipeline_layout_info);
  }

  if (!vk_pipeline_layout) {
    throw std::runtime_error("Could not create pipeline layout!");
  }

  return vk_pipeline_layout;
}
