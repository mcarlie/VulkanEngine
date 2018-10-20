#ifndef UTILS_H
#define UTILS_H

#define _USE_MATH_DEFINES 
#include <math.h>

#include <Eigen/Eigen>

namespace VulkanEngine {

  /// Contains definitions of various mathematical constants.
  namespace Constants {
  
    /// Definition of pi to use in application.
    template< typename T >
    constexpr T pi() { return static_cast< T >( M_PI ); };

    /// Definition of e to use in application.
    template< typename T >
    constexpr T e() { return static_cast< T >( M_E ); };

  }

}

#endif /* UTILS_H */