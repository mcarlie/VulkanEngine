// Copyright (c) 2025 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef VERTEXATTRIBUTE_CPP
#define VERTEXATTRIBUTE_CPP

#include <VulkanEngine/VertexAttribute.h>

template <typename T>
VulkanEngine::VertexAttribute<T>::VertexAttribute(const T* data,
                                                  size_t _num_elements,
                                                  uint32_t _location,
                                                  vk::Format _format)
    : Attribute(_num_elements, sizeof(T),
                vk::BufferUsageFlagBits::eVertexBuffer),
      location(_location),
      format(_format) {
  updateBuffer(data, sizeof(T) * num_elements);
}

template <typename T>
VulkanEngine::VertexAttribute<T>::~VertexAttribute() {}

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
