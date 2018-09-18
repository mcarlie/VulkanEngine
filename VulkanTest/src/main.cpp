#define TINYOBJLOADER_IMPLEMENTATION

#include <VulkanTest/GLFWWindow.h>
#include <VulkanTest/VulkanManager.h>
#include <VulkanTest/VertexAttribute.h>
#include <VulkanTest/Camera.h>
#include <VulkanTest/Mesh.h>
#include <VulkanTest/OBJLoader.h>

#include <iostream>
#include <vector>

int main() {

    std::shared_ptr< VulkanTest::Window > window( new VulkanTest::GLFWWindow( 800, 600, "VulkanTest", false ) );
    window->initialize();

    auto vulkan_manager = VulkanTest::VulkanManager::getInstance();
    vulkan_manager->initialize( window );

    while( !window->shouldClose() ) {
      vulkan_manager->drawImage();
      window->update();
    }

    return 0;
}
