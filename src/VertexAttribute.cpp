#ifndef VERTEXATTRIBUTE_CPP
#define VERTEXATTRIBUTE_CPP

#include <VulkanEngine/VertexAttribute.h>

template <typename T>
VulkanEngine::VertexAttribute<T>::VertexAttribute(const T *data,
                                                  size_t _num_elements,
                                                  uint32_t _location,
                                                  vk::Format _format)
    : Attribute(_num_elements, sizeof(T),
                vk::BufferUsageFlagBits::eVertexBuffer),
      location(_location), format(_format) {
  updateBuffer(data, sizeof(T) * num_elements);
}

template <typename T> VulkanEngine::VertexAttribute<T>::~VertexAttribute() {}

template <typename T>
const vk::VertexInputBindingDescription
VulkanEngine::VertexAttribute<T>::getVkVertexInputBindingDescription(
    uint32_t binding_index) const {
  return vk::VertexInputBindingDescription()
      .setBinding(binding_index)
      .setInputRate(vk::VertexInputRate::eVertex)
      .setStride(sizeof(T));
}

template <typename T>
const vk::VertexInputAttributeDescription
VulkanEngine::VertexAttribute<T>::getVkVertexInputAttributeDescriptions(
    uint32_t binding_index) const {
  return vk::VertexInputAttributeDescription()
      .setBinding(binding_index)
      .setLocation(location)
      .setFormat(getVkFormat())
      .setOffset(0);
}

template <typename T>
vk::Format VulkanEngine::VertexAttribute<T>::getVkFormat() const {
  return format;
}

#endif /* VERTEXATTRIBUTE_CPP */