#include <VulkanEngine/Descriptor.h>

VulkanEngine::Descriptor::Descriptor( 
  uint32_t _binding,
  uint32_t _desciptor_count,
  vk::DescriptorType _vk_descriptor_type,
  vk::ShaderStageFlags _vk_shader_stage_flags ) 
  : binding( _binding ), descriptor_count( _desciptor_count ), vk_descriptor_type( _vk_descriptor_type ), vk_shader_stage_flags( _vk_shader_stage_flags ) {
}

VulkanEngine::Descriptor::~Descriptor() {
}

const vk::DescriptorSetLayoutBinding VulkanEngine::Descriptor::getVkDescriptorSetLayoutBinding() {

  return vk::DescriptorSetLayoutBinding()
    .setBinding( binding )
    .setDescriptorCount( descriptor_count )
    .setDescriptorType( vk_descriptor_type )
    .setStageFlags( vk_shader_stage_flags );

}

const vk::DescriptorPoolSize VulkanEngine::Descriptor::getVkDescriptorPoolSize() {
  
  return vk::DescriptorPoolSize()
    .setDescriptorCount( descriptor_count )
    .setType( vk_descriptor_type );

}