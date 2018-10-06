#ifndef OBJLOADER_CPP
#define OBJLOADER_CPP

#include <VulkanTest/OBJLoader.h>
#include <VulkanTest/VertexAttribute.h>
#include <VulkanTest/Utilities.h>
#include <VulkanTest/Mesh.h>

#include <Eigen/Eigen>
#include <tiny_obj_loader.h>

#include <unordered_map>
#include <chrono>
#include <iostream>

namespace VulkanTest {

  namespace OBJLoader {
 
    template< typename IndexType, typename ... AdditionalAttributeTypes >
    std::shared_ptr< MeshBase > getShape( const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib ) {

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

      using MeshType = Mesh< 
        Eigen::Vector3f,
        IndexType,
        Eigen::Vector3f,
        Eigen::Vector2f,
        AdditionalAttributeTypes ... >;

      std::shared_ptr< VertexAttribute< Eigen::Vector3f > > position_attribute( 
        new VertexAttribute< Eigen::Vector3f >( positions.data(), positions.size(), 0, vk::Format::eR32G32B32Sfloat ) );
    
      std::shared_ptr< IndexAttribute< IndexType > > index_attribute(
        new IndexAttribute< IndexType >( indices.data(), indices.size() ) );

      MeshType::AttributeContainer< Eigen::Vector3f > normal_attribute;
      if( !normals.empty() ) {
        normal_attribute.emplace_back(
          new VertexAttribute< Eigen::Vector3f >( normals.data(), normals.size(), 1, vk::Format::eR32G32B32Sfloat ) );
      }

      MeshType::AttributeContainer< Eigen::Vector2f > texcoord_attribute;
      if( !texcoords.empty() ){
        texcoord_attribute.emplace_back(
          new VertexAttribute< Eigen::Vector2f >( texcoords.data(), texcoords.size(), 2, vk::Format::eR32G32Sfloat ) );
      }

      // AttributeContainer for additional attributes (normals and texcoords)
      std::tuple<
        MeshType::AttributeContainer< Eigen::Vector3f >, 
        MeshType::AttributeContainer< Eigen::Vector2f > > 
        additional_attributes(
          normal_attribute,
          texcoord_attribute );

      MeshType* mesh = new MeshType();
      mesh->setPositions( position_attribute );
      mesh->setIndices( index_attribute );
      mesh->setAttributes( additional_attributes );

      return std::shared_ptr< MeshBase >( mesh );

    }
  
  }

}

template< typename ... AdditionalAttributeTypes >
std::vector< std::shared_ptr< VulkanTest::MeshBase > > VulkanTest::OBJLoader::loadOBJ( 
  const char* obj_path,
  const char* base_path,
  AdditionalAttributeTypes ... additional_attributes ) {

  auto begin = std::chrono::system_clock::now();
  std::vector< std::shared_ptr< MeshBase > > meshes;

  tinyobj::attrib_t attrib;
  std::vector< tinyobj::shape_t > shapes;
  std::vector< tinyobj::material_t > materials;
  std::string err;
  
  // TODO Get rid of need to triangulate
  if( !tinyobj::LoadObj( &attrib, &shapes, &materials, &err, obj_path, base_path, true ) ) {
    throw std::runtime_error( "Could not load obj file: " + std::string( obj_path ) + ", " + err );
  }

  for( const auto& shape : shapes ) {

    // We represent the indices using uint16_t unless that is not sufficient given the number of indices
    if( shape.mesh.indices.size() > std::numeric_limits< uint16_t >::max() ) {
      meshes.push_back( getShape< uint32_t >( shape, attrib ) );
    } else {
      meshes.push_back( getShape< uint16_t >( shape, attrib ) );
    }

  }

  auto time = std::chrono::duration_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() - begin ).count();
  std::cout << "Mesh loading time: " << time << "(ns)" << std::endl;

  return meshes;

}

#endif /* OBJLOADER_CPP */