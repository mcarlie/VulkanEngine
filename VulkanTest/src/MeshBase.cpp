#ifndef MESH_CPP
#define MESH_CPP

#include <VulkanTest/MeshBase.h>

VulkanTest::MeshBase::MeshBase() {
}

VulkanTest::MeshBase::MeshBase( const std::shared_ptr< Shader > _shader ) : shader( _shader ) {
}

VulkanTest::MeshBase::~MeshBase() {
}

const std::shared_ptr< VulkanTest::Shader >& VulkanTest::MeshBase::getShader() {
  return shader;
}

void VulkanTest::MeshBase::setShader(const std::shared_ptr< VulkanTest::Shader >& _shader) {
  shader = _shader;
}

#endif /* MESH_CPP */