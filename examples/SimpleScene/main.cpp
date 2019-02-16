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
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    0.1f,
    1000.0f,
    45.0f,
    window->getWidth(),
    window->getHeight() ) );
  
  camera->setTranform( Eigen::Affine3f( Eigen::Translation3f( 0.0f, 0.0f, -5.0f ) ).matrix() );

  scene_children.push_back( camera );

  std::shared_ptr< VulkanEngine::OBJMesh > obj_mesh( new VulkanEngine::OBJMesh( 
    "/Users/michael/Desktop/VK/models/spider_pumpkin_obj.obj", "/Users/michael/Desktop/VK/models/" ) );
  
  Eigen::Affine3f transform( Eigen::Translation3f( 0.0f, 0.0f, 5.0f ) );
  transform *= Eigen::Scaling( 0.05f );
  obj_mesh->setTranform( transform.matrix() );
  
  scene_children.push_back( obj_mesh );

  scene->addChildren( scene_children );

  const auto& keyboard_input = window->getKeyboardInput();
  
  const float camera_move_speed = 0.1;
  
  while( !window->shouldClose() ) {

    if( keyboard_input.get() ) {
      
      Eigen::Affine3f camera_transform = Eigen::Affine3f( camera->getTransform() );
      
      const auto key_left = keyboard_input->getLastKeyStatus( GLFW_KEY_LEFT );
      const auto key_right = keyboard_input->getLastKeyStatus( GLFW_KEY_RIGHT );
      
      const auto key_up = keyboard_input->getLastKeyStatus( GLFW_KEY_UP );
      const auto key_down = keyboard_input->getLastKeyStatus( GLFW_KEY_DOWN );
      
      if( key_left == VulkanEngine::KeyboardInput::PRESSED || key_left == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_transform.translation()[0] -= camera_move_speed;
      }
      
      if( key_right == VulkanEngine::KeyboardInput::PRESSED || key_right == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_transform.translation()[0] += camera_move_speed;
      }
      
      if( key_up == VulkanEngine::KeyboardInput::PRESSED || key_up == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_transform.translation()[1] += camera_move_speed;
      }
      
      if( key_down == VulkanEngine::KeyboardInput::PRESSED || key_down == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_transform.translation()[1] -= camera_move_speed;
      }
      
      camera->setTranform( camera_transform.matrix() );
      
    }
    
    VulkanEngine::SceneState scene_state;
    scene->update( scene_state );
    
    vulkan_manager->drawImage();
    window->update();

  }

  return 0;

}
