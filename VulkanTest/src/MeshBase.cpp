#ifndef MESH_CPP
#define MESH_CPP

#include <VulkanEngine/MeshBase.h>

VulkanEngine::MeshBase::MeshBase() {
}

VulkanEngine::MeshBase::MeshBase( const std::shared_ptr< Shader > _shader ) : shader( _shader ) {
}

VulkanEngine::MeshBase::~MeshBase() {
}

const std::shared_ptr< VulkanEngine::Shader >& VulkanEngine::MeshBase::getShader() {
  return shader;
}

void VulkanEngine::MeshBase::setShader(const std::shared_ptr< VulkanEngine::Shader >& _shader) {
  shader = _shader;
}

#endif /* MESH_CPP */