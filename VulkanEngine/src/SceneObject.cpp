#include <VulkanEngine/SceneObject.h>

VulkanEngine::SceneObject::SceneObject()
    : transform(Eigen::Matrix4f::Identity()) {}

/// Update this scene object.
/// \param scene_state Contains information about the current state of the
/// scene.
void VulkanEngine::SceneObject::preUpdate(SceneState &scene_state) {

  scene_state.setTransform(scene_state.getTotalTransform() * transform);
}

void VulkanEngine::SceneObject::update(SceneState &scene_state) {

  /// Update all children
  for (const auto &c : children) {
    c->preUpdate(scene_state);
    c->update(scene_state);
    c->postUpdate(scene_state);
  }
}

/// Update this scene object.
/// \param scene_state Contains information about the current state of the
/// scene.
void VulkanEngine::SceneObject::postUpdate(SceneState &scene_state) {

  scene_state.setTransform(scene_state.getTotalTransform() *
                           transform.inverse());
}

void VulkanEngine::SceneObject::addChildren(
    const std::vector<std::shared_ptr<SceneObject>> &_children) {
  children.insert(children.end(), _children.cbegin(), _children.cend());
}

const Eigen::Matrix4f VulkanEngine::SceneObject::getTransform() const {
  return transform;
}

void VulkanEngine::SceneObject::setTranform(const Eigen::Matrix4f &_transform) {
  transform = _transform;
}
