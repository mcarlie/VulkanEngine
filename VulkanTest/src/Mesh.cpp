#ifndef MESH_CPP
#define MESH_CPP

#include <VulkanTest/Mesh.h>

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh() {
  static_assert( sizeof( IndexType ) == sizeof( uint16_t ) || sizeof( IndexType ) == sizeof( uint32_t ),
    "Mesh IndexType template parameter must be the same size as either uint16_t or uint32_t" );
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh( 
  const std::shared_ptr< VertexAttribute< PositionType > >& _positions,
  const std::shared_ptr< IndexAttribute< IndexType > >& _indices,
  const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes,
  const std::shared_ptr< Shader >& _shader
  ) : Mesh(), MeshBase( _shader ), positions( _positions ), indices( _indices ), attributes( _attributes ) {
}
    
template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::~Mesh() {
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setPositions( 
  const std::shared_ptr< VertexAttribute< PositionType > >& _positions ) {
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
const vk::PipelineVertexInputStateCreateInfo VulkanTest::Mesh< 
  PositionType, IndexType, AdditionalAttributeTypes ... >::getVkPipelineVertexInputStateCreateInfo() {

  binding_descriptions.push_back( positions->getVkVertexInputBindingDescription() );
  attribute_descriptions.push_back( positions->getVkVertexInputAttributeDescriptions() );

  Utilities::tupleForEach( additional_attributes, [ this ]( const auto& attrib_vec ){
    for( const auto& attrib : attrib_vec ) {
      if( attrib.get() ) {
        binding_descriptions.push_back( attrib->getVkVertexInputBindingDescription() );
        attribute_descriptions.push_back( attrib->getVkVertexInputAttributeDescriptions() );      
      }
    } 
  } );

  return vk::PipelineVertexInputStateCreateInfo()
    .setPVertexBindingDescriptions( binding_descriptions.data() )
    .setVertexBindingDescriptionCount( static_cast< uint32_t >( binding_descriptions.size() ) )
    .setPVertexAttributeDescriptions( attribute_descriptions.data() )
    .setVertexAttributeDescriptionCount( static_cast< uint32_t >( attribute_descriptions.size() ) );

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
const vk::PipelineInputAssemblyStateCreateInfo VulkanTest::Mesh< 
  PositionType, IndexType, AdditionalAttributeTypes ... >::getVkPipelineInputAssemblyStateCreateInfo() {

  return vk::PipelineInputAssemblyStateCreateInfo()
      .setPrimitiveRestartEnable( VK_FALSE )
      .setTopology( vk::PrimitiveTopology::eTriangleList );

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::transferBuffers(
  const vk::CommandBuffer& command_buffer ) {

  if( !positions.get() ) {
    throw std::runtime_error( "No position vertex buffer to transfer for mesh!" );
  }
  
  positions->transferBuffer();

  if( indices.get() ) {
    indices->transferBuffer();
  }

  Utilities::tupleForEach( additional_attributes, []( const auto& attrib_vec ){
    for( const auto& attrib : attrib_vec ) {
      if( attrib.get() ) {
        attrib->transferBuffer();
      }
    } 
  } );

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::bindVertexBuffers( const vk::CommandBuffer& command_buffer ) {

  if( !positions.get() ) {
    throw std::runtime_error( "No position vertex buffer to bind for mesh!" );
  }

  std::vector< vk::Buffer > buffers;
  buffers.push_back( positions->getVkBuffer() );

  Utilities::tupleForEach( additional_attributes, [ &buffers ]( const auto& attrib_vec ){
    for( const auto& attrib : attrib_vec ) {
      if( attrib.get() ) {
        buffers.push_back( attrib->getVkBuffer() );
      }
    } 
  } );

  command_buffer.bindVertexBuffers( 0, buffers, { 0 } );

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::bindIndexBuffer( const vk::CommandBuffer& command_buffer ) {

  if( indices.get() ) {
    command_buffer.bindIndexBuffer( 
      indices->getVkBuffer(), 0, sizeof( IndexType ) == sizeof( uint16_t ) ? vk::IndexType::eUint16 : vk::IndexType::eUint32 );
  }

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::draw( const vk::CommandBuffer& command_buffer ) {

  if( indices.get() ) {
    command_buffer.drawIndexed( static_cast< uint32_t >( indices->getNumElements() ), 1, 0, 0, 0 );
  } else {
    command_buffer.draw( static_cast< uint32_t >( positions->getNumElements() ), 1, 0, 0 );
  }

}

#endif /* MESH_CPP */