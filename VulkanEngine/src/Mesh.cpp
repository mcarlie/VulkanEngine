#ifndef MESH_CPP
#define MESH_CPP

#include <VulkanEngine/Mesh.h>
#include <VulkanEngine/Utilities.h>

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh() {
}
    
template< typename PositionType, typename IndexType, class ...AdditionalAttributeTypes >
VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::Mesh(
  const std::shared_ptr< VertexAttribute< PositionType > >& _positions,
  const std::shared_ptr< IndexAttribute< IndexType > >& _indices,
  //const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes,
  const std::shared_ptr< Shader >& _shader ) {
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::~Mesh() {
}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setPositions( 
  const std::shared_ptr< VertexAttribute< PositionType > >& _positions ) {
  positions = _positions;
}
    
template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setIndices( 
  const std::shared_ptr< IndexAttribute< IndexType > >& _indices ) {
  indices = _indices;
}
    
//template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
//void VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::setAttributes(
//  const std::tuple< AttributeContainer< AdditionalAttributeTypes > ... >& _attributes ) {
//  attributes = _attributes;
//}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
const vk::PipelineVertexInputStateCreateInfo& VulkanEngine::Mesh<
  PositionType, IndexType, AdditionalAttributeTypes ... >::getVkPipelineVertexInputStateCreateInfo() {

  uint32_t binding_index = 0;

  binding_descriptions.push_back( positions->getVkVertexInputBindingDescription( binding_index ) );
  attribute_descriptions.push_back( positions->getVkVertexInputAttributeDescriptions( binding_index ) );

  auto visitor = [ this, &binding_index ]( const auto& attrib_vec ){
    for( const auto& attrib : attrib_vec ) {
      if( attrib.get() ) {
        ++binding_index;
        binding_descriptions.push_back( attrib->getVkVertexInputBindingDescription( binding_index ) );
        attribute_descriptions.push_back( attrib->getVkVertexInputAttributeDescriptions( binding_index ) );
      }
    }
  };
    
  Utilities::tupleForEach( attributes, visitor );

  pipeline_vertex_input_state_info = vk::PipelineVertexInputStateCreateInfo()
    .setPVertexBindingDescriptions( binding_descriptions.data() )
    .setVertexBindingDescriptionCount( static_cast< uint32_t >( binding_descriptions.size() ) )
    .setPVertexAttributeDescriptions( attribute_descriptions.data() )
    .setVertexAttributeDescriptionCount( static_cast< uint32_t >( attribute_descriptions.size() ) );
    
  return pipeline_vertex_input_state_info;

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
const vk::PipelineInputAssemblyStateCreateInfo& VulkanEngine::Mesh< 
  PositionType, IndexType, AdditionalAttributeTypes ... >::getVkPipelineInputAssemblyStateCreateInfo() {

  pipeline_input_assembly_state_info = vk::PipelineInputAssemblyStateCreateInfo()
      .setPrimitiveRestartEnable( VK_FALSE )
      .setTopology( vk::PrimitiveTopology::eTriangleList );
    
  return pipeline_input_assembly_state_info;

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::transferBuffers(
  const vk::CommandBuffer& command_buffer ) {

  if( !positions.get() ) {
    throw std::runtime_error( "No position vertex buffer to transfer for mesh!" );
  }
  
  positions->transferBuffer();

  if( indices.get() ) {
    indices->transferBuffer();
  }

  auto visitor = []( const auto& attrib_vec ) {
    for( const auto& attrib : attrib_vec ) {
      if( attrib.get() ) {
        attrib->transferBuffer();
      }
    }
  };
  
  Utilities::tupleForEach( attributes, visitor );

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::bindVertexBuffers( const vk::CommandBuffer& command_buffer ) {

  if( !positions.get() ) {
    throw std::runtime_error( "No position vertex buffer to bind for mesh!" );
  }

  std::vector< vk::Buffer > buffers;
  buffers.push_back( positions->getVkBuffer() );

  auto visitor = [ &buffers ]( const auto& attrib_vec ) {
    for( const auto& attrib : attrib_vec ) {
      if( attrib.get() ) {
        buffers.push_back( attrib->getVkBuffer() );
      }
    }
  };
  
  Utilities::tupleForEach( attributes, visitor );

  std::vector< vk::DeviceSize > offsets( buffers.size(), 0 );
  command_buffer.bindVertexBuffers( 0, buffers, offsets );

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::bindIndexBuffer( const vk::CommandBuffer& command_buffer ) {

  if( indices.get() ) {
    command_buffer.bindIndexBuffer( 
      indices->getVkBuffer(), 0, sizeof( IndexType ) == sizeof( uint16_t ) ? vk::IndexType::eUint16 : vk::IndexType::eUint32 );
  }

}

template< typename PositionType, typename IndexType, class ... AdditionalAttributeTypes >
void VulkanEngine::Mesh< PositionType, IndexType, AdditionalAttributeTypes ... >::draw( const vk::CommandBuffer& command_buffer ) {

  if( indices.get() ) {
    command_buffer.drawIndexed( static_cast< uint32_t >( indices->getNumElements() ), 1, 0, 0, 0 );
  } else {
    command_buffer.draw( static_cast< uint32_t >( positions->getNumElements() ), 1, 0, 0 );
  }

}

#endif /* MESH_CPP */
