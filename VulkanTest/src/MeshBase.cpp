#ifndef MESH_CPP
#define MESH_CPP

#include <VulkanTest/MeshBase.h>

VulkanTest::MeshBase::MeshBase() {
}

VulkanTest::MeshBase::MeshBase( const std::shared_ptr< Shader > _shader ) : Renderable( _shader )  {
}

VulkanTest::MeshBase::~MeshBase() {
}

#endif /* MESH_CPP */