#ifndef VERTEXATTRIBUTE_H
#define VERTEXATTRIBUTE_H

#include <vulkan/vulkan.hpp>

#include <Eigen/Eigen>

#include <vector>

namespace VulkanTest {

  /// Class which represents a vertex attribute
  /// \tparam T The scalar type to use
  /// \tparam elements_x The number of elements in dimension x
  /// \tparam elements_y the number of elements in dimension y
  template< typename T, int elements_x, int elements_y >
  class VertexAttribute {

  public:

    /// Constructor
    /// \param data The vertex data which will be represented by this VertexAttribute instance
    VertexAttribute( const std::vector< Eigen::Matrix< T, elements_x, elements_y > >& _data, uint32_t _binding ) 
      : binding( _binding ), num_vertices( static_cast< uint32_t >( _data.size() ) ) {

      const size_t size = sizeof( _data[0] ) * _data.size();

      auto renderer = Renderer::get();
      vk::Device& vk_device = renderer->getVkDevice();

      auto buffer_info = vk::BufferCreateInfo()
        .setSize( size )
        .setUsage( vk::BufferUsageFlagBits::eVertexBuffer )
        .setSharingMode( vk::SharingMode::eExclusive );

      vk_buffer = vk_device.createBuffer( buffer_info );
      vk::MemoryRequirements vk_memory_requirements = vk_device.getBufferMemoryRequirements( vk_buffer );

      auto vk_allocate_info = vk::MemoryAllocateInfo()
        .setAllocationSize( vk_memory_requirements.size )
        .setMemoryTypeIndex( renderer->findMemoryTypeIndex( vk_memory_requirements.memoryTypeBits,
          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent ) );

      vk::DeviceMemory vk_device_memory = vk_device.allocateMemory( vk_allocate_info );
      vk_device.bindBufferMemory( vk_buffer, vk_device_memory, 0 );

      void* data_ptr = vk_device.mapMemory( vk_device_memory, 0, size );
      std::memcpy( data_ptr, _data.data(), size );
      vk_device.unmapMemory( vk_device_memory );

    }

    /// Destructor
    ~VertexAttribute() {
      Renderer::get()->getVkDevice().destroyBuffer( vk_buffer );
    };

    /// \return The number of vertices in this VertexAttribute
    uint32_t getNumVertices() {
      return num_vertices;
    }

    const vk::Buffer& getVkBuffer() {
      return vk_buffer;
    }

    /// \return The Vulkan VertexInputBindingDescription for this VertexAttribute instance
    const vk::VertexInputBindingDescription getVkVertexInputBindingDescription() {

      return vk::VertexInputBindingDescription()
        .setBinding( binding )
        .setInputRate( vk::VertexInputRate::eVertex )
        .setStride( sizeof( Eigen::Matrix< T, elements_x, elements_y > ) );

    }

  private:

    uint32_t binding;
    uint32_t num_vertices;
    vk::Buffer vk_buffer;
    vk::MemoryRequirements vk_memory_requirements;

  };

}

#endif /* VERTEXATTRIBUTE_H */