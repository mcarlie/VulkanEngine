#define TINYOBJLOADER_IMPLEMENTATION

#include <VulkanTest/GLFWWindow.h>
#include <VulkanTest/UniformBuffer.h>
#include <VulkanTest/ShaderImage.h>
#include <VulkanTest/StagedBuffer.h>
#include <VulkanTest/OBJLoader.h>

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct MvpUbo {
  Eigen::Matrix4f model;
  Eigen::Matrix4f view;
  Eigen::Matrix4f projection;
};

using RGBATexture2D1S 
  = VulkanTest::StagedBuffer< VulkanTest::ShaderImage< vk::Format::eR8G8B8A8Unorm, vk::ImageType::e2D, vk::ImageTiling::eOptimal, vk::SampleCountFlagBits::e1 > >;

int main() {

  std::shared_ptr< VulkanTest::Window > window( new VulkanTest::GLFWWindow( 800, 600, "VulkanTest", false ) );
  window->initialize();

  auto vulkan_manager = VulkanTest::VulkanManager::getInstance();
  vulkan_manager->initialize( window );

  std::shared_ptr< VulkanTest::MeshBase > mesh;
  mesh = VulkanTest::OBJLoader::loadOBJ( "C:/Users/Michael/Desktop/VK/VulkanTest/models/spider_pumpkin_obj.obj", "" )[0];

  std::shared_ptr< VulkanTest::ShaderModule > fragment_shader( new VulkanTest::ShaderModule( "C:/Users/Michael/Desktop/VK/VulkanTest/shaders/frag.spv", vk::ShaderStageFlagBits::eFragment ) );
  std::shared_ptr< VulkanTest::ShaderModule > vertex_shader( new VulkanTest::ShaderModule( "C:/Users/Michael/Desktop/VK/VulkanTest/shaders/vert.spv", vk::ShaderStageFlagBits::eVertex ) );
  std::shared_ptr< VulkanTest::Shader > shader( new VulkanTest::Shader( { fragment_shader, vertex_shader } ) );

  mesh->setShader( shader );

  int texture_width;
  int texture_height;
  int channels_in_file;
  unsigned char* image_data = stbi_load( "C:/Users/Michael/Desktop/VK/VulkanTest/models/spider_pumpkin_obj_0.jpg", &texture_width, &texture_height, &channels_in_file, 4 );
  std::shared_ptr< RGBATexture2D1S > texture;
  texture.reset( new RGBATexture2D1S( 
    vk::ImageLayout::eUndefined,
    vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
    VMA_MEMORY_USAGE_GPU_ONLY,
    static_cast< uint32_t >( texture_width ),
    static_cast< uint32_t >( texture_height ),
    1, sizeof( unsigned char ) * 4, 1,
    1, // TODO
    vk::DescriptorType::eCombinedImageSampler,
    vk::ShaderStageFlagBits::eFragment ) );

  texture->setImageData( image_data );
  texture->createImageView( vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor );
  texture->createSampler();

  std::vector< std::shared_ptr< VulkanTest::UniformBuffer< MvpUbo > > > uniform_buffers;
  uniform_buffers.resize( 3 );
  for( auto& ub : uniform_buffers ) {
    ub.reset( new VulkanTest::UniformBuffer< MvpUbo >( 0 ) );
  }

  std::vector< std::vector< std::shared_ptr< VulkanTest::Descriptor > > > descriptors;
  for( size_t i = 0; i < 3; ++i ) {
    descriptors.push_back( { texture, uniform_buffers[i] } );
  }

  shader->setDescriptors( descriptors );

  std::shared_ptr< VulkanTest::Camera< float > > camera;
  camera.reset( new VulkanTest::Camera< float >( 
    { 0, 300, -100 },
    { 0, 0, 0 },
    { 0, 1, 0 },
    0.1,
    1000,
    45,
    window->getWidth(),
    window->getHeight() ) );

  vulkan_manager->createGraphicsPipeline( mesh );
  vulkan_manager->createCommandBuffers( mesh );

  mesh->transferBuffers();
  texture->transferBuffer();

  while( !window->shouldClose() ) {

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
