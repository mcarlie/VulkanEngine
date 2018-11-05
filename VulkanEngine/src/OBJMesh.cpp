#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Mesh.h>
#include <VulkanEngine/Utilities.h>
#include <VulkanEngine/ShaderImage.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <chrono>

namespace OBJMeshInternal {

  template< typename IndexType >
  std::shared_ptr< VulkanEngine::MeshBase > getShape( const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib ) {

    using Vertex = std::tuple< const Eigen::Vector3f&, const Eigen::Vector3f&, const Eigen::Vector2f& >;
    std::unordered_map< Vertex, size_t > unique_vertices;

    const auto default_normal = Eigen::Vector3f( 0.0f, 0.0f, 0.0f );
    const auto default_texcoord = Eigen::Vector2f( 0.0f, 0.0f );

    std::vector< Eigen::Vector3f > positions;
    // Important to reserve these since we are using references to elements to keep track of unique vertices.
    // If not reserved the vector might be relocated in memory which invalidates the references.
    positions.reserve( shape.mesh.indices.size() );

    std::vector< IndexType > indices;
    indices.reserve( shape.mesh.indices.size() );

    std::vector< Eigen::Vector3f > normals;
    normals.reserve( shape.mesh.indices.size() );

    std::vector< Eigen::Vector2f > texcoords;
    texcoords.reserve( shape.mesh.indices.size() );

    for( size_t i = 0; i < shape.mesh.indices.size(); ++i ) {
      
      positions.emplace_back(
        attrib.vertices[ 3 * shape.mesh.indices[i].vertex_index + 0 ],
        attrib.vertices[ 3 * shape.mesh.indices[i].vertex_index + 1 ],
        attrib.vertices[ 3 * shape.mesh.indices[i].vertex_index + 2 ]
      );

      if( shape.mesh.indices[i].normal_index > -1 ) {
        normals.emplace_back(
          attrib.normals[ 3 * shape.mesh.indices[i].normal_index + 0 ],
          attrib.normals[ 3 * shape.mesh.indices[i].normal_index + 1 ],
          attrib.normals[ 3 * shape.mesh.indices[i].normal_index + 2 ]
        );
      }

      if( shape.mesh.indices[i].texcoord_index > -1 ) {
        texcoords.emplace_back(
          attrib.texcoords[ 2 * shape.mesh.indices[i].texcoord_index + 0 ],
          1.0f - attrib.texcoords[ 2 * shape.mesh.indices[i].texcoord_index + 1 ]
        );
      }

      Vertex vertex = std::forward_as_tuple( 
        positions.back(),
        normals.empty() ? default_normal : normals.back(),
        texcoords.empty() ? default_texcoord : texcoords.back() );

      if( unique_vertices.count( vertex ) == 0 ) {
        unique_vertices[ vertex ] = positions.size() - 1;
      } else {
        positions.pop_back();
        if( !normals.empty() ) {
          normals.pop_back();
        }
        if( !texcoords.empty() ) {
          texcoords.pop_back();
        }
      }

      indices.push_back( static_cast< IndexType >( unique_vertices[ vertex ] ) );

    }

    using MeshType = VulkanEngine::Mesh<
    Eigen::Vector3f,
    IndexType,
    Eigen::Vector3f,
    Eigen::Vector2f >;

    std::shared_ptr< VulkanEngine::VertexAttribute< Eigen::Vector3f > > position_attribute( 
      new VulkanEngine::VertexAttribute< Eigen::Vector3f >( positions.data(), positions.size(), 0, vk::Format::eR32G32B32Sfloat ) );
    
    std::shared_ptr< VulkanEngine::IndexAttribute< IndexType > > index_attribute(
      new VulkanEngine::IndexAttribute< IndexType >( indices.data(), indices.size() ) );

    typename MeshType::template AttributeContainer< Eigen::Vector3f > normal_attribute;
    if( !normals.empty() ) {
      normal_attribute.emplace_back(
        new VulkanEngine::VertexAttribute< Eigen::Vector3f >( normals.data(), normals.size(), 1, vk::Format::eR32G32B32Sfloat ) );
    }

    typename MeshType::template AttributeContainer< Eigen::Vector2f > texcoord_attribute;
    if( !texcoords.empty() ){
      texcoord_attribute.emplace_back(
        new VulkanEngine::VertexAttribute< Eigen::Vector2f >( texcoords.data(), texcoords.size(), 2, vk::Format::eR32G32Sfloat ) );
    }

    // AttributeContainer for additional attributes (normals and texcoords)
    std::tuple<
    typename MeshType::template AttributeContainer< Eigen::Vector3f >,
    typename MeshType::template AttributeContainer< Eigen::Vector2f > >
      additional_attributes(
        normal_attribute,
        texcoord_attribute );

    MeshType* mesh = new MeshType();
    mesh->setPositions( position_attribute );
    mesh->setIndices( index_attribute );
    mesh->setAttributes( additional_attributes );

    return std::shared_ptr< VulkanEngine::MeshBase >( mesh );

  }

}

VulkanEngine::OBJMesh::OBJMesh( 
  const std::string& obj_file,
  const std::string& mtl_path,
  const std::shared_ptr< Shader > _shader ) : shader( _shader ) {

  // Load mesh
  loadOBJ( obj_file.c_str(), mtl_path.c_str() );

  // Transfer mesh vertex data to GPU
  for( const auto& m : meshes ){
    m->transferBuffers();
  }

  // Create a shader if one isn't provided
  if( !shader.get() ) {
    std::shared_ptr< ShaderModule > fragment_shader( 
      new ShaderModule( "/Users/michael/Desktop/VK/shaders/frag.spv", vk::ShaderStageFlagBits::eFragment ) );
    std::shared_ptr< ShaderModule > vertex_shader( 
      new ShaderModule( "/Users/michael/Desktop/VK/shaders/vert.spv", vk::ShaderStageFlagBits::eVertex ) );
    shader.reset( new Shader( { fragment_shader, vertex_shader } ) );
  }
  
  int texture_width;
  int texture_height;
  int channels_in_file;
  unsigned char* image_data = stbi_load( "/Users/michael/Desktop/VK/models/spider_pumpkin_obj_0.jpg",
                                        &texture_width, &texture_height,
                                        &channels_in_file, 4 );
  
  using RGBATexture2D1S
  = VulkanEngine::StagedBuffer<
  VulkanEngine::ShaderImage<
  vk::Format::eR8G8B8A8Unorm,
  vk::ImageType::e2D,
  vk::ImageTiling::eOptimal,
  vk::SampleCountFlagBits::e1 > >;
  
  std::shared_ptr< RGBATexture2D1S > texture;
  texture.reset( new RGBATexture2D1S( vk::ImageLayout::eUndefined,
                                      vk::ImageUsageFlagBits::eTransferDst
                                      | vk::ImageUsageFlagBits::eTransferSrc
                                      | vk::ImageUsageFlagBits::eSampled, /// TODO These could be template parameters instead
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
  texture->transferBuffer();
  
  // TODO baseclass which handles pipeline
  VulkanEngine::VulkanManager::getInstance()->createGraphicsPipeline( meshes[0], shader );
  
  struct MvpUbo {
    Eigen::Matrix4f model;
    Eigen::Matrix4f view;
    Eigen::Matrix4f projection;
  };
  
  std::vector< std::shared_ptr< VulkanEngine::UniformBuffer< MvpUbo > > > uniform_buffers;
  uniform_buffers.resize( 3 );
  
  std::vector< std::vector< std::shared_ptr< VulkanEngine::Descriptor > > > descriptors;
  for( size_t i = 0; i < 3; ++i ) {
    descriptors.push_back( { texture, uniform_buffers[i] } );
  }

  shader->setDescriptors( descriptors );
  
}

void VulkanEngine::OBJMesh::updateCallback( SceneState& scene_state ) {

}

void VulkanEngine::OBJMesh::loadOBJ( const char* obj_path, const char* mtl_path ) {

  auto begin = std::chrono::system_clock::now();

  tinyobj::attrib_t attrib;
  std::vector< tinyobj::shape_t > shapes;
  std::vector< tinyobj::material_t > materials;
  std::string err;
  
  // TODO Get rid of need to triangulate using primitive restart
  if( !tinyobj::LoadObj( &attrib, &shapes, &materials, &err, obj_path, mtl_path, true ) ) {
    throw std::runtime_error( "Could not load obj file: " + std::string( obj_path ) + ", " + err );
  }

  for( const auto& shape : shapes ) {

    // We represent the indices using uint16_t unless that is not sufficient given the number of indices
    if( shape.mesh.indices.size() > std::numeric_limits< uint16_t >::max() ) {
      meshes.push_back( OBJMeshInternal::getShape< uint32_t >( shape, attrib ) );
    } else {
      meshes.push_back( OBJMeshInternal::getShape< uint16_t >( shape, attrib ) );
    }

  }

  using RGBATexture2D1S 
    = VulkanEngine::StagedBuffer< 
    VulkanEngine::ShaderImage< 
    vk::Format::eR8G8B8A8Unorm,
    vk::ImageType::e2D,
    vk::ImageTiling::eOptimal,
    vk::SampleCountFlagBits::e1 > >;

  for( const auto& material : materials ) {

    if( material.diffuse_texname != "" ) {

      int texture_width;
      int texture_height;
      int channels_in_file;
      unsigned char* image_data = stbi_load( 
        material.diffuse_texname.c_str(),
        &texture_width, &texture_height,
        &channels_in_file, 4 );

      if( image_data ) {

        std::shared_ptr< RGBATexture2D1S > diffuse_texture( 
          new RGBATexture2D1S( 
            vk::ImageLayout::eUndefined,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled, /// TODO These could be template parameters instead
            VMA_MEMORY_USAGE_GPU_ONLY,
            static_cast< uint32_t >( texture_width ),
            static_cast< uint32_t >( texture_height ),
            1, sizeof( unsigned char ) * 4, 1,
            1, // TODO
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment ) );

        diffuse_texture->setImageData( image_data );
        diffuse_texture->createImageView( vk::ImageViewType::e2D, vk::ImageAspectFlagBits::eColor );
        diffuse_texture->createSampler();
        textures[material.diffuse_texname] = diffuse_texture;

      } else {
        std::cerr << "OBJMesh texture: " << material.diffuse_texname << " could not be loaded" << std::endl;
      }

    }

  }

  auto time = std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() - begin ).count();
  std::cout << "Mesh loading time: " << time << "(ns)" << std::endl;

}
