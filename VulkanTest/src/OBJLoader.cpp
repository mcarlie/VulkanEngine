#ifndef OBJLOADER_CPP
#define OBJLOADER_CPP

#include <VulkanTest/OBJLoader.h>
#include <VulkanTest/VertexAttribute.h>
#include <VulkanTest/Utils.h>
#include <VulkanTest/Mesh.h>

#include <Eigen/Eigen>
#include <tiny_obj_loader.h>

#include <unordered_map>

namespace VulkanTest {

  template< typename IndexType, typename ... AdditionalAttributeTypes >
  std::shared_ptr< MeshBase > getShape( const tinyobj::shape_t& s, const tinyobj::attrib_t& attrib ) {

    using Vertex = std::tuple< const Eigen::Vector3f&, const Eigen::Vector3f&, const Eigen::Vector2f& >;
    std::unordered_map< Vertex, size_t > unique_vertices;

    std::vector< Eigen::Vector3f > positions;
    // Important to reserve these since we are using references to elements to keep track of unique vertices.
    // If not reserved the vector might be relocated in memory which invalidates the references.
    positions.reserve( attrib.vertices.size() / 3 );

    std::vector< Eigen::Vector3f > normals;
    if( !attrib.normals.empty() ) {
      normals.reserve( attrib.normals.size() / 3 );
    }

    std::vector< Eigen::Vector2f > tex_coords;
    if( !attrib.texcoords.empty() ) {
      tex_coords.reserve( attrib.texcoords.size() / 2 );
    }

    std::vector< IndexType > indices;

    for( size_t i = 0; i < s.mesh.indices.size(); ++i ) {
      
      positions.push_back( {
        attrib.vertices[ 3 * s.mesh.indices[i].vertex_index + 0 ],
        attrib.vertices[ 3 * s.mesh.indices[i].vertex_index + 1 ],
        attrib.vertices[ 3 * s.mesh.indices[i].vertex_index + 2 ]
      } );

      //if( i.normal_index > 0 ) {
      //  normals.push_back( {
      //    attrib.normals[ 3 * i.normal_index + 0 ],
      //    attrib.normals[ 3 * i.normal_index + 1 ],
      //    attrib.normals[ 3 * i.normal_index + 2 ]
      //  } );
      //}

      //if( i.texcoord_index > 0 ) {
      //  tex_coords.push_back( {
      //    attrib.texcoords[ 2 * i.texcoord_index + 0 ],
      //    attrib.texcoords[ 2 * i.texcoord_index + 1 ]
      //  } );
      //}

      Vertex vertex = std::forward_as_tuple( 
        positions.back(),
        normals.empty() ? Eigen::Vector3f( 0.0f, 0.0f, 0.0f ) : normals.back(),
        tex_coords.empty() ? Eigen::Vector2f( 0.0f, 0.0f ) : tex_coords.back() );

      if( unique_vertices.count( vertex ) == 0 ) {
        unique_vertices[ vertex ] = positions.size() - 1;
      } else {
        positions.pop_back();
      }

      indices.push_back( static_cast< IndexType >( unique_vertices[ vertex ] ) );

    }

    std::shared_ptr< VertexAttribute< Eigen::Vector3f > > position_attribute( 
      new VertexAttribute< Eigen::Vector3f >( positions.data(), positions.size(), 0, vk::Format::eR32G32B32Sfloat ) );
    
    std::shared_ptr< IndexAttribute< IndexType > > index_attribute(
      new IndexAttribute< IndexType >( indices.data(), indices.size() ) );

    std::shared_ptr< VertexAttribute< Eigen::Vector3f > > normal_attribute;
    std::shared_ptr< VertexAttribute< Eigen::Vector2f > > uv_coordinate_attribute;

    using MeshType = Mesh< 
      float,
      IndexType,
      Eigen::Vector3f,
      Eigen::Vector3f,
      Eigen::Vector2f,
      AdditionalAttributeTypes ... >;

    MeshType* mesh = new MeshType();
    mesh->setPositions( position_attribute );
    mesh->setIndices( index_attribute );

    return std::shared_ptr< MeshBase >( mesh );

  }

}

template< typename ... AdditionalAttributeTypes >
std::vector< std::shared_ptr< VulkanTest::MeshBase > > VulkanTest::loadOBJ( 
  const char* obj_path,
  const char* base_path,
  AdditionalAttributeTypes ... additional_attributes ) {

  std::vector< std::shared_ptr< MeshBase > > meshes;

  tinyobj::attrib_t attrib;
  std::vector< tinyobj::shape_t > shapes;
  std::vector< tinyobj::material_t > materials;
  std::string err;
  
  // TODO Get rid of need to triangulate
  if( !tinyobj::LoadObj( &attrib, &shapes, &materials, &err, obj_path, base_path, true ) ) {
    throw std::runtime_error( "Could not load obj file: " + std::string( obj_path ) + ", " + err );
  }

  for( const auto& s : shapes ) {

    // We represent the indices using uint16_t unless that is not sufficient given the number of indices
    if( s.mesh.indices.size() > std::numeric_limits< uint16_t >::max() ) {
      meshes.push_back( getShape< uint32_t >( s, attrib ) );
    } else {
      meshes.push_back( getShape< uint16_t >( s, attrib ) );
    }

  }

  return meshes;

}

#endif /* OBJLOADER_CPP */