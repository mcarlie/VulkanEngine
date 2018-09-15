#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <vulkan/vulkan.hpp>

#include <string>

namespace VulkanTest {
  
  /// An abstract base class for any object which can be rendered
  class Renderable {

  public:

    Renderable();
    ~Renderable();



  };

}

#endif /* RENDERABLE_H */