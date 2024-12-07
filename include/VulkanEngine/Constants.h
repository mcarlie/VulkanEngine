// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef UTILS_H
#define UTILS_H

#include <Eigen/Eigen>

namespace VulkanEngine {

/// Contains definitions of various mathematical constants.
namespace Constants {

/// Definition of pi to use in application.
template <typename T> constexpr T pi() {
  return static_cast<T>(3.14159265358979323846264338327950288419716939937510L);
};

/// Definition of e to use in application.
template <typename T> constexpr T e() {
  return static_cast<T>(2.71828182845904523536028747135266249775724709369995L);
};

} // namespace Constants

} // namespace VulkanEngine

#endif /* UTILS_H */
