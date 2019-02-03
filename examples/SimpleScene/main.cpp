#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/Utilities.h>

#include <iostream>
#include <vector>

int main() {

  std::shared_ptr< VulkanEngine::Window > window( new VulkanEngine::GLFWWindow( 1280, 800, "VulkanEngine", false ) );
  window->initialize();
  
  auto vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  vulkan_manager->initialize( window );
  
  std::shared_ptr< VulkanEngine::Scene > scene( new VulkanEngine::Scene() );
  std::vector< std::shared_ptr< VulkanEngine::SceneObject > > scene_children;

  std::shared_ptr< VulkanEngine::MouseInput > mouse_input = window->getMouseInput();

  std::shared_ptr< VulkanEngine::Camera> camera;
  camera.reset( new VulkanEngine::Camera(
    { 0.0f, 0.0f, -5.0f  },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    0.1f,
    1000.0f,
    45.0f,
    window->getWidth(),
    window->getHeight() ) );

  scene_children.push_back( camera );

  std::shared_ptr< VulkanEngine::OBJMesh > obj_mesh( new VulkanEngine::OBJMesh( 
    "C:/Users/Michael/Desktop/VK/models/teapot.obj", "C:/Users/Michael/Desktop/VK/models/" ) );

  scene_children.push_back( obj_mesh );

  scene->addChildren( scene_children );

  double camera_yaw = 0.0;
  double camera_pitch = 0.0;

  while( !window->shouldClose() ) {

    if( mouse_input->leftButtonPressed() ){

      double x, y, px, py;
      mouse_input->getPosition( x, y );
      mouse_input->getPreviousPosition( px, py );

      camera_yaw += ( x - px ) * 0.05f;
      camera_pitch += ( py - y ) * 0.05f;

      if( camera_pitch > 89.0f ) {
        camera_pitch = 89.0f;
      }
      if( camera_pitch < -89.0f ) {
        camera_pitch = -89.0f;
      }

      Eigen::Vector3f camera_dir(
        std::cos( VulkanEngine::Utilities::toRadians< double >( camera_yaw ) ) 
        * std::cos( VulkanEngine::Utilities::toRadians< double >( camera_pitch ) ),
        std::sin( VulkanEngine::Utilities::toRadians< double >( camera_pitch ) ),
        std::sin( VulkanEngine::Utilities::toRadians< double >( camera_yaw ) ) 
        * std::cos( VulkanEngine::Utilities::toRadians< double >( camera_pitch ) )
      );
      camera_dir.normalized();

      camera->setLookAt( camera->getPosition() + camera_dir );

    }

    VulkanEngine::SceneState scene_state;
    scene->update( scene_state );
    
    vulkan_manager->drawImage();
    window->update();

  }

  return 0;

}
