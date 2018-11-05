#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/Utilities.h>

#include <iostream>
#include <vector>

struct MvpUbo {
  Eigen::Matrix4f model;
  Eigen::Matrix4f view;
  Eigen::Matrix4f projection;
};

int main() {

  std::shared_ptr< VulkanEngine::Window > window( new VulkanEngine::GLFWWindow( 1280, 800, "VulkanEngine", false ) );
  window->initialize();
  
  auto vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  vulkan_manager->initialize( window );
  
  std::shared_ptr< VulkanEngine::Scene > scene( new VulkanEngine::Scene() );
  
  std::shared_ptr< VulkanEngine::OBJMesh > obj_mesh( new VulkanEngine::OBJMesh( "/Users/michael/Desktop/VK/models/spider_pumpkin_obj.obj", "/Users/michael/Desktop/VK/models/" ) );

  std::shared_ptr< VulkanEngine::MouseInput > mouse_input = window->getMouseInput();

  std::shared_ptr< VulkanEngine::Camera> camera;
  camera.reset( new VulkanEngine::Camera(
    { 0.0f, 5.0f, -5.0f  },
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    0.1f,
    1000.0f,
    45.0f,
    window->getWidth(),
    window->getHeight() ) );

//  vulkan_manager->createGraphicsPipeline( mesh );
//  vulkan_manager->createCommandBuffers( mesh );
//
//  mesh->transferBuffers();
//  texture->transferBuffer();

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
        std::cos( VulkanEngine::Utilities::toRadians< double >( camera_yaw ) ) * std::cos( VulkanEngine::Utilities::toRadians< double >( camera_pitch ) ),
        std::sin( VulkanEngine::Utilities::toRadians< double >( camera_pitch ) ),
        std::sin( VulkanEngine::Utilities::toRadians< double >( camera_yaw ) ) * std::cos( VulkanEngine::Utilities::toRadians< double >( camera_pitch ) )
      );
      camera_dir.normalized();

      camera->setLookAt( camera->getPosition() + camera_dir );

    }

    MvpUbo vp_data;
    vp_data.projection = camera->getPerspectiveProjectionMatrix();
    vp_data.view = camera->getViewMatrix();
//    for( auto& ub : uniform_buffers ) {
//      ub->updateBuffer( &vp_data, sizeof( vp_data ) );
//    }
    vulkan_manager->drawImage();
    window->update();

  }

  return 0;

}
