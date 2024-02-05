#ifndef INDEXATTRIBUTE_H
#define INDEXATTRIBUTE_H

#include <VulkanEngine/Attribute.h>

#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Represents an index attribute for indexed rendering.
/// \tparam T The type to use for index elements.
template <typename T> class IndexAttribute : public Attribute {

public:
  /// Constructor.
  /// \param data The index data.
  /// \param num_elements The number of indice.
  IndexAttribute(const T *data, size_t num_elements);

  /// Destructor.
  virtual ~IndexAttribute();
};

} // namespace VulkanEngine

#include <IndexAttribute.cpp>

#endif /* INDEXATTRIBUTE_H */
