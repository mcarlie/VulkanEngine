#include <VulkanEngine/GLFWWindow.h>
#include <VulkanEngine/UniformBuffer.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/StagedBuffer.h>
#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/Utilities.h>

#include <iostream>
#include <vector>

#include <stb_image.h>

struct MvpUbo {
  Eigen::Matrix4f model;
  Eigen::Matrix4f view;
  Eigen::Matrix4f projection;
};

using RGBATexture2D1S 
  = VulkanEngine::StagedBuffer< 
  VulkanEngine::ShaderImage< 
  vk::Format::eR8G8B8A8Unorm,
  vk::ImageType::e2D,
  vk::ImageTiling::eOptimal,
  vk::SampleCountFlagBits::e1 > >;

int main() {

  std::shared_ptr< VulkanEngine::Scene > scene( new VulkanEngine::Scene() );
  std::shared_ptr< VulkanEngine::OBJMesh > obj_mesh( new VulkanEngine::OBJMesh( "C:/Users/Michael/Desktop/VK/models/spider_pumpkin_obj.obj", "C:/Users/Michael/Desktop/VK/models/" ) );

  std::shared_ptr< VulkanEngine::Window > window( new VulkanEngine::GLFWWindow( 1280, 800, "VulkanEngine", false ) );
  window->initialize();

  std::shared_ptr< VulkanEngine::MouseInput > mouse_input = window->getMouseInput();

  auto vulkan_manager = VulkanEngine::VulkanManager::getInstance();
  vulkan_manager->initialize( window );

  //int texture_width;
  //int texture_height;
  //int channels_in_file;
  //unsigned char* image_data = stbi_load( 
  //  "C:/Users/Michael/Desktop/VK/models/chalet.jpg",
  //  &texture_width, &texture_height,
  //  &channels_in_file, 4 );

  //std::shared_ptr< RGBATexture2D1S > texture;
  //texture.reset( new RGBATexture2D1S( 
  //  vk::ImageLayout::eUndefined,
  //  vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled, /// TODO These could be template parameters instead
  //  VMA_MEMORY_USAGE_GPU_ONLY,
  //  static_cast< uint32_t >( texture_width ),
  //  static_cast< uint32_t >( texture_height ),
  //  1, sizeof( unsigned char ) * 4, 1,
  //  1, // TODO
  //  vk::DescriptorType::eCombinedImageSampler,
  //  vk::ShaderStageFlagBits::eFragment ) );

  //texture->setImageData( image_data );
  //texture->createImageView( vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor );
  //texture->createSampler();

  std::vector< std::shared_ptr< VulkanEngine::UniformBuffer< MvpUbo > > > uniform_buffers;
  uniform_buffers.resize( 3 );



//  std::vector< std::vector< std::shared_ptr< VulkanEngine::Descriptor > > > descriptors;
//  for( size_t i = 0; i < 3; ++i ) {
//    descriptors.push_back( { texture, uniform_buffers[i] } );
//  }

  /*shader->setDescriptors( descriptors );*/

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

  //vulkan_manager->createGraphicsPipeline( mesh );
  //vulkan_manager->createCommandBuffers( mesh );

  //mesh->transferBuffers();
  //texture->transferBuffer();

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
    for( auto& ub : uniform_buffers ) {
      ub->updateBuffer( &vp_data, sizeof( vp_data ) );
    }
    vulkan_manager->drawImage();
    window->update();

  }

  return 0;

}
