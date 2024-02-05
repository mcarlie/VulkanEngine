#include <VulkanEngine/OBJMesh.h>
#include <VulkanEngine/VulkanManager.h>
#include <VulkanEngine/Mesh.h>
#include <VulkanEngine/Utilities.h>
#include <VulkanEngine/ShaderImage.h>
#include <VulkanEngine/Scene.h>

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
  std::filesystem::path obj_file,
  std::filesystem::path mtl_path,
  const std::shared_ptr< Shader > _shader ) :
  SceneObject(),
  GraphicsPipeline(),
  shader( _shader ),
  graphics_pipeline_updated( false ) {
    
  std::error_code obj_file_error;
  if( !std::filesystem::exists( obj_file, obj_file_error ) ) {
    std::cout << "Provided obj path " + ( obj_file.string() ) + " is invalid. Error code: "
      << obj_file_error.value() << " " << obj_file_error.message() << std::endl;
    return;
  }
  
  std::error_code mtl_path_error;
  if( !mtl_path.empty() ) {
    if( !std::filesystem::exists( mtl_path, mtl_path_error ) ) {
      std::cout << "Provided mtl path " + mtl_path.string() + " is invalid. Error code: "
        << mtl_path_error.value() << " " << mtl_path_error.message() << std::endl;
    }
    mtl_path /= ""; // Appends a / on POSIX systems. tinyobj doesn't recognize the path otherwise
  } else {
    auto obj_file_copy = obj_file;
    mtl_path = obj_file_copy.remove_filename();
  }

  // Load mesh
  loadOBJ( obj_file.string().c_str(), mtl_path.string().c_str() );

  // Transfer mesh vertex data to GPU
  for( const auto& m : meshes ) {
    m->transferBuffers();
  }
  
}

VulkanEngine::OBJMesh::~OBJMesh() {
  
}

void VulkanEngine::OBJMesh::update( SceneState& scene_state ) {

  MvpUbo ubo_data;
  ubo_data.projection = scene_state.getProjectionMatrix();
  ubo_data.view = scene_state.getViewMatrix();
  ubo_data.model = scene_state.getTotalTransform();
  
  for( auto& ub : mvp_buffers ) {
    ub->updateBuffer( &ubo_data, sizeof( ubo_data ) );
  }
  
  auto vulkan_manager = VulkanManager::getInstance();
  
  const std::shared_ptr< Window >& window
    = scene_state.getScene().getActiveWindow();
  if( graphics_pipeline_updated ) {
    graphics_pipeline_updated = !window->sizeHasChanged();
  }
  
  if( !graphics_pipeline_updated ) {
    int32_t width = window->getFramebufferWidth();
    int32_t height = window->getFramebufferHeight();
    setViewPort( 0, 0, static_cast< float >( width ), static_cast< float >( height ), 0.0f, 1.0f );
    setScissor( 0, 0, width, height );
    createGraphicsPipeline( meshes[0], shader );
    graphics_pipeline_updated = true;
  }
  
  bindPipeline();

  auto current_command_buffer = vulkan_manager->getCurrentCommandBuffer();
  
  for( auto& mesh : meshes ) {
    mesh->bindVertexBuffers( current_command_buffer );
    mesh->bindIndexBuffer( current_command_buffer );
    if( shader.get() ) {
      shader->bindDescriptorSet( current_command_buffer, static_cast< uint32_t >( vulkan_manager->getCurrentFrame() ) ); /// TODO 0 was i for each frame in flight
    }

    mesh->draw( current_command_buffer );
  }
  
  SceneObject::update( scene_state );
  
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

  mvp_buffers.resize( VulkanManager::getInstance()->getFramesInFlight() ); /// TODO should be dependent on number of frames in flight
  for( auto& ub : mvp_buffers ) {
	  ub.reset( new VulkanEngine::UniformBuffer< MvpUbo >( 0 ) );
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

  }
  
  // Create a shader if one isn't provided
  if( !shader.get() ) {
    std::shared_ptr< ShaderModule > vertex_shader(
      new ShaderModule( "/Users/michael.carlie/Dev/vk/shaders/vert.spv", true, vk::ShaderStageFlagBits::eVertex ) );
    std::shared_ptr< ShaderModule > fragment_shader(
      new ShaderModule( "/Users/michael.carlie/Dev/vk/shaders/frag.spv", true, vk::ShaderStageFlagBits::eFragment ) );
    shader.reset( new Shader( { fragment_shader, vertex_shader } ) );
  }

  std::vector< std::vector< std::shared_ptr< Descriptor > > > descriptors;
  for( size_t i = 0; i < VulkanManager::getInstance()->getFramesInFlight(); ++i ) {
    std::vector< std::shared_ptr< Descriptor > > frame_descriptors;
    for( const auto& t : textures ) {
      frame_descriptors.push_back( t.second );
    }
    frame_descriptors.push_back( mvp_buffers[i] );
    descriptors.push_back( frame_descriptors );
  }

  shader->setDescriptors( descriptors );

  auto time = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now() - begin ).count();
  std::cout << "Mesh loading time: " << time << "(ms)" << std::endl;

}

const std::string VulkanEngine::OBJMesh::getVertexShaderString(bool has_tex_coords) {
  std::stringstream return_string;
  
  return_string
  << std::endl
  << "#version 450" << std::endl
  << "#extension GL_ARB_separate_shader_objects : enable" << std::endl
  
  << "layout(binding = 0) uniform UniformBufferObject {" << std::endl
  << "  mat4 model;" << std::endl
  << "  mat4 view;" << std::endl
  << "  mat4 proj;" << std::endl
  << "} ubo;" << std::endl

  << "layout(location = 0) in vec3 inPosition;" << std::endl;
  
  // if (has_tex_coords) {
  //   return_string
  //   << "layout(location = 2) in vec2 inTexcoords;" << std::endl
  //   << "layout(location = 2) out vec2 outTexcoords;" << std::endl;
  // }

  return_string
  << "out gl_PerVertex {" << std::endl
  << "  vec4 gl_Position;" << std::endl
  << "};" << std::endl

  << "void main() {" << std::endl
  << "  gl_Position = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);" << std::endl
  // << "  outTexcoords = inTexcoords;" << std::endl
  << "}" << std::endl;
  
  return return_string.str();
}

const std::string VulkanEngine::OBJMesh::getFragmentShaderString(bool has_tex_coords) {
  std::stringstream return_string;
  
  return_string
  << "#version 450" << std::endl
  << "#extension GL_ARB_separate_shader_objects : enable" << std::endl;

  // if (has_tex_coords) {
  //   return_string
  //   << "layout(location = 2) in vec2 inTexcoords;" << std::endl;
  // }

  return_string
  << "layout(location = 0) out vec4 outColor;" << std::endl;

  // TODO should be if there is a texture
  // if (has_tex_coords) {
  //   return_string
  //   << "layout(binding = 1) uniform sampler2D texSampler;" << std::endl;
  // }

  return_string
  << "void main() {" << std::endl;
  // if (has_tex_coords) {
  //   return_string
  //   << "  outColor = texture( texSampler, inTexcoords );" << std::endl;
  // } else {
    return_string
    << "  outColor = vec4(1.0, 1.0, 1.0, 1.0);" << std::endl;
  // }
  return_string
  << "}" << std::endl;

  return return_string.str();
}
