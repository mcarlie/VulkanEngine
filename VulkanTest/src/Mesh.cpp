#ifndef MESH_CPP
#define MESH_CPP

#include <VulkanTest/Mesh.h>

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh() {
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh( 
  const std::shared_ptr< VertexAttribute< Eigen::Matrix< PositionType, 3, 1 > > >& _positions,
  const std::shared_ptr< IndexAttribute< IndexType > >& _indices,
  const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes,
  const std::shared_ptr< Shader >& _shader
  ) : positions( _positions ), indices( _indices ), attributes( _attributes ), shader( _shader ) {
}
    
template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::~Mesh() {
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setPositions( 
  const std::shared_ptr< VertexAttribute< Eigen::Matrix< PositionType, 3, 1 > > >& _positions ) {
  positions = _positions;
}
    
template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setIndices( 
  const std::shared_ptr< IndexAttribute< IndexType > >& _indices ) {
  indices = _indices;
}
    
template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setAttributes( 
  const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes ) {
  attributes = _attributes;
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setShader( 
  const std::shared_ptr< Shader >& _shader ) {
  shader = _shader;
}

#endif /* MESH_CPP */