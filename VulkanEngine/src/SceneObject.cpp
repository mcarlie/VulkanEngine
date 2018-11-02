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

void VulkanEngine::SceneObject::updateCallback( SceneState& scene_state ) {
}