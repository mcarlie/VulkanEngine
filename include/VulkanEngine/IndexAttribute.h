// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_INDEXATTRIBUTE_H_
#define INCLUDE_VULKANENGINE_INDEXATTRIBUTE_H_

#include <VulkanEngine/Attribute.h>

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Represents an index attribute for indexed rendering.
/// \tparam T The type to use for index elements.
template <typename T>
class IndexAttribute : public Attribute {
 public:
  /// Constructor.
  /// \param data The index data.
  /// \param num_elements The number of indice.
  IndexAttribute(const T* data, size_t num_elements);

  /// Destructor.
  virtual ~IndexAttribute();
};

}  // namespace VulkanEngine

#include <IndexAttribute.cpp>  // NOLINT(build/include)

#endif  // INCLUDE_VULKANENGINE_INDEXATTRIBUTE_H_
