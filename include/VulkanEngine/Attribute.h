// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_ATTRIBUTE_H_
#define INCLUDE_VULKANENGINE_ATTRIBUTE_H_

#include <VulkanEngine/StagedBuffer.h>

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Abstract base class for shader attributes
class Attribute : public StagedBuffer<Buffer> {
 public:
  /// Constructor.
  /// \param _num_elements The number of elements in this attribute buffer
  /// \param _element_size The size of a single element in the attribute buffer
  /// \param flags The attribute's vk::BufferUsageFlags
  Attribute(size_t _num_elements, size_t _element_size,
            vk::BufferUsageFlags flags);

  /// Destructor.
  ~Attribute();

  /// \return The number of elements in this Attribute.
  size_t getNumElements() const;

 protected:
  /// The number of elements in this attribute.
  size_t num_elements;

  /// The data size of the elements which this attribute is represented by.
  size_t element_size;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_ATTRIBUTE_H_
