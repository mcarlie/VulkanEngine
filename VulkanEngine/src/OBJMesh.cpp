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
  for( const auto& m : meshes ) {
    m->transferBuffers();
  }
  
  // TODO baseclass which handles pipeline
  VulkanEngine::VulkanManager::getInstance()->createGraphicsPipeline( meshes[0], shader );
  VulkanEngine::VulkanManager::getInstance()->createCommandBuffers( meshes[0], shader );
  
}

VulkanEngine::OBJMesh::~OBJMesh() {
  
}

void VulkanEngine::OBJMesh::updateCallback( SceneState& scene_state ) {

  MvpUbo ubo_data;
  ubo_data.projection = scene_state.getProjectionMatrix();
  ubo_data.view = scene_state.getViewMatrix();
  ubo_data.model = scene_state.getTotalTransform();
  
  for( auto& ub : mvp_buffers ) {
    ub->updateBuffer( &ubo_data, sizeof( ubo_data ) );
  }
  
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

  for( const auto& material : materials ) {

    if( material.diffuse_texname != "" ) {

      int texture_width;
      int texture_height;
      int channels_in_file;
      unsigned char* image_data = stbi_load( 
        ( std::string( mtl_path ) + material.diffuse_texname ).c_str(),
        &texture_width, &texture_height,
        &channels_in_file, 4 );

      if( image_data ) {

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
        textures[ material.diffuse_texname ] = texture;

      } else {
        std::cerr << "OBJMesh texture: " << material.diffuse_texname << " could not be loaded" << std::endl;
      }

    }
    
    // Create a shader if one isn't provided
    const char* shader_path = std::getenv( "VULKAN_ENGINE_SHADERS_DIR" );
    if( !shader.get() && shader_path ) {
      std::shared_ptr< ShaderModule > fragment_shader(
        new ShaderModule( std::string( shader_path ) + std::string( "/frag.spv" ), vk::ShaderStageFlagBits::eFragment ) );
      std::shared_ptr< ShaderModule > vertex_shader(
        new ShaderModule( std::string( shader_path ) + std::string( "/vert.spv" ), vk::ShaderStageFlagBits::eVertex ) );
      shader.reset( new Shader( { fragment_shader, vertex_shader } ) );
    }
    
    mvp_buffers.resize( 3 ); /// TODO should be dependent on number of frames in flight
    for( auto& ub : mvp_buffers ) {
      ub.reset( new VulkanEngine::UniformBuffer< MvpUbo >( 0 ) );
    }
    
    std::vector< std::vector< std::shared_ptr< Descriptor > > > descriptors;
    for( size_t i = 0; i < 3; ++i ) { /// TODO should be dependent on number of frames in flight
      std::vector< std::shared_ptr< Descriptor > > frame_descriptors;
      for( const auto& t : textures ) {
        frame_descriptors.push_back( t.second );
      }
      frame_descriptors.push_back( mvp_buffers[i] );
      descriptors.push_back( frame_descriptors );
    }
    
    shader->setDescriptors( descriptors );

  }

  auto time = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now() - begin ).count();
  std::cout << "Mesh loading time: " << time << "(ms)" << std::endl;

}
