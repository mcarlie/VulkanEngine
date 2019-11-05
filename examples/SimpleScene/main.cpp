#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/Utilities.h>

#include <iostream>
#include <vector>

int main( int argc, char** argv ) {
  
  std::shared_ptr< VulkanEngine::Window > window( new VulkanEngine::GLFWWindow( 1280, 800, "VulkanEngine", false ) );
  window->initialize();
  
  auto vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  vulkan_manager->initialize( window );
  
  std::shared_ptr< VulkanEngine::Scene > scene( new VulkanEngine::Scene() );
  std::vector< std::shared_ptr< VulkanEngine::SceneObject > > scene_children;

  std::shared_ptr< VulkanEngine::MouseInput > mouse_input = window->getMouseInput();

  std::shared_ptr< VulkanEngine::Camera> camera;
  camera.reset( new VulkanEngine::Camera(
    { 0.0f, 0.0f, 0.1f },
    { 0.0f, 1.0f, 0.0f },
    0.1f,
    1000.0f,
    45.0f,
    window->getWidth(),
    window->getHeight() ) );
  
  camera->setTranform( Eigen::Affine3f( Eigen::Translation3f( 0.0f, 0.0f, 5.0f ) ).matrix() );

  std::shared_ptr< VulkanEngine::SceneObject > camera_container( new VulkanEngine::SceneObject() );
  camera_container->addChildren( { camera } );
  
  scene_children.push_back( { camera_container } );
  
  if(argc > 1) {
    std::string obj_path( argv[1] );
    std::string mtl_path;
    if( argc > 2 ) {
      mtl_path = argv[2];
    }
    std::shared_ptr< VulkanEngine::OBJMesh > obj_mesh(
      new VulkanEngine::OBJMesh( obj_path, mtl_path ) );
    
    Eigen::Affine3f transform( Eigen::Translation3f( 0.0f, 0.0f, 0.0f ) );
    transform *= Eigen::Scaling( 0.025f );
    obj_mesh->setTranform( transform.matrix() );
    
    scene_children.push_back( obj_mesh );
  }

  scene->addChildren( scene_children );

  const auto& keyboard_input = window->getKeyboardInput();
  
  const float camera_move_speed = 0.03;
  
  while( !window->shouldClose() ) {

    if( keyboard_input.get() ) {
      
      Eigen::Affine3f camera_transform = Eigen::Affine3f( camera->getTransform() );
      
      const auto key_left = keyboard_input->getLastKeyStatus( GLFW_KEY_LEFT );
      const auto key_right = keyboard_input->getLastKeyStatus( GLFW_KEY_RIGHT );
      
      const auto key_up = keyboard_input->getLastKeyStatus( GLFW_KEY_UP );
      const auto key_down = keyboard_input->getLastKeyStatus( GLFW_KEY_DOWN );
      
      float camera_rotation_x = 0.0f;
      float camera_rotation_y = 0.0f;
      
      if( key_left == VulkanEngine::KeyboardInput::PRESSED || key_left == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_rotation_x += camera_move_speed;
      }
      
      if( key_right == VulkanEngine::KeyboardInput::PRESSED || key_right == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_rotation_x -= camera_move_speed;
      }
      
      if( key_up == VulkanEngine::KeyboardInput::PRESSED || key_up == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_rotation_y -= camera_move_speed;
      }
      
      if( key_down == VulkanEngine::KeyboardInput::PRESSED || key_down == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_rotation_y += camera_move_speed;
      }
      
      if( camera_rotation_x != 0.0f || camera_rotation_y != 0.0f ) {
        Eigen::AngleAxisf camera_rot_x( camera_rotation_x, camera->getUpVector().normalized() );
        
        Eigen::Vector3f camera_position = camera_transform.translation();
        Eigen::Vector3f look_at = camera->getLookAt();
        
        Eigen::Vector3f look_at_to_pos = camera_position - look_at;
        
        look_at_to_pos = camera_rot_x * look_at_to_pos;
        
        Eigen::AngleAxisf camera_rot_y(
          camera_rotation_y, ( look_at_to_pos.cross( camera->getUpVector() ) ).normalized() );
        
        look_at_to_pos = camera_rot_y * look_at_to_pos;
        
        camera->setTranform( Eigen::Affine3f( Eigen::Translation3f( look_at_to_pos + look_at ) ).matrix() );
      }
      
      const auto key_w = keyboard_input->getLastKeyStatus( GLFW_KEY_W );
      const auto key_a = keyboard_input->getLastKeyStatus( GLFW_KEY_A );
      
      const auto key_s = keyboard_input->getLastKeyStatus( GLFW_KEY_S );
      const auto key_d = keyboard_input->getLastKeyStatus( GLFW_KEY_D );
      
      const auto key_z = keyboard_input->getLastKeyStatus( GLFW_KEY_Z );
      const auto key_x = keyboard_input->getLastKeyStatus( GLFW_KEY_X );
      
      Eigen::Vector3f camera_movement = { 0.0f, 0.0f, 0.0f };
      
      if( key_a == VulkanEngine::KeyboardInput::PRESSED || key_a == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_movement( 0 ) += camera_move_speed;
      }
      
      if( key_d == VulkanEngine::KeyboardInput::PRESSED || key_d == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_movement( 0 ) -= camera_move_speed;
      }
      
      if( key_z == VulkanEngine::KeyboardInput::PRESSED || key_z == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_movement( 1 ) += camera_move_speed;
      }
      
      if( key_x == VulkanEngine::KeyboardInput::PRESSED || key_x == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_movement( 1 ) -= camera_move_speed;
      }
      
      if( key_w == VulkanEngine::KeyboardInput::PRESSED || key_w == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_movement( 2 ) += camera_move_speed;
      }
      
      if( key_s == VulkanEngine::KeyboardInput::PRESSED || key_s == VulkanEngine::KeyboardInput::REPEAT ) {
        camera_movement( 2 ) -= camera_move_speed;
      }
      
      camera->setLookAt( camera->getLookAt() + camera_movement );
      camera->setTranform( Eigen::Affine3f( Eigen::Translation3f( camera_movement )
                                           * Eigen::Affine3f( camera->getTransform() ) ).matrix() );
      
    }
    
    scene->update();
    
    vulkan_manager->drawImage();
    window->update();

  }

  return 0;

}
