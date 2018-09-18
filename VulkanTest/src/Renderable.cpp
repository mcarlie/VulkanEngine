#include <VulkanTest/Renderable.h>

VulkanTest::Renderable::Renderable() {
}

VulkanTest::Renderable::Renderable( const std::shared_ptr< Shader >& _shader ) : shader( _shader ) {
}

VulkanTest::Renderable::~Renderable() {
}

void VulkanTest::Renderable::setShader( const std::shared_ptr< Shader >& _shader ) {
  shader = _shader;
}