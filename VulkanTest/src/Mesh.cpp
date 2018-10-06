#ifndef MESH_CPP
#define MESH_CPP

#include <VulkanTest/Mesh.h>

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh() {
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanTest::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh( 
  const std::shared_ptr< VertexAttribute< PositionType > >& _positions,
  const std::shared_ptr< IndexAttribute< IndexType > >& _indices,
  const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes,
  const std::shared_ptr< Shader >& _shader
  ) : MeshBase( _shader ), positions( _positions ), indices( _indices ), attributes( _attributes ) {
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
const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _additional_attributes ) {
  additional_attributes = _additional_attributes;
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
const vk::PipelineVertexInputStateCreateInfo VulkanTest::Mesh< 
  PositionType, IndexType, AdditionalAttributeTypes ... >::getVkPipelineVertexInputStateCreateInfo() {

  uint32_t binding_index = 0;

  binding_descriptions.push_back( positions->getVkVertexInputBindingDescription( binding_index ) );
  attribute_descriptions.push_back( positions->getVkVertexInputAttributeDescriptions( binding_index ) );

  Utilities::tupleForEach( additional_attributes, [ this, &binding_index ]( const auto& attrib_vec ){
    for( const auto& attrib : attrib_vec ) {
      if( attrib.get() ) {
        ++binding_index;
        binding_descriptions.push_back( attrib->getVkVertexInputBindingDescription( binding_index ) );
        attribute_descriptions.push_back( attrib->getVkVertexInputAttributeDescriptions( binding_index ) );      
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

  std::vector< vk::DeviceSize > offsets( buffers.size(), 0 );
  command_buffer.bindVertexBuffers( 0, buffers, offsets );

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