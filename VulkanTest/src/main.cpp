#include <VulkanTest/GLFWWindow.h>
#include <VulkanTest/Renderer.h>
#include <VulkanTest/VertexAttribute.h>

#include <iostream>
#include <vector>

int main() {

    std::shared_ptr< VulkanTest::Window > window( new VulkanTest::GLFWWindow( 800, 600, "VulkanTest", false ) );
    window->initialize();

    auto renderer = VulkanTest::Renderer::get();
    renderer->initialize( window );

    while( !window->shouldClose() ) {
      renderer->drawImage();
      window->update();
    }

    return 0;
}
