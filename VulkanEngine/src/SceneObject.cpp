#include <VulkanEngine/SceneObject.h>

VulkanEngine::SceneObject::SceneObject() : transform( Eigen::Matrix4f::Identity() ) {
}

void VulkanEngine::SceneObject::update( SceneState& scene_state ) {

  scene_state.setTransform( scene_state.getTotalTransform() * transform );

  updateCallback( scene_state );

  /// Update all children
  for( const auto& c : children ) {
    c->update( scene_state );
  }

  scene_state.setTransform( scene_state.getTotalTransform() * transform.inverse() );

}

void VulkanEngine::SceneObject::addChildren( const std::vector< std::shared_ptr< SceneObject > >& _children ) {
  children.insert( children.end(), _children.cbegin(), _children.cend() );
}

void VulkanEngine::SceneObject::updateCallback( SceneState& scene_state ) {
}

const Eigen::Matrix4f& VulkanEngine::SceneObject::getTransform() {
  return transform;
}

void VulkanEngine::SceneObject::setTranform( const Eigen::Matrix4f& _transform ) {
  transform = _transform;
}
