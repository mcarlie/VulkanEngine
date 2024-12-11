#include <VulkanEngine/SceneObject.h>

VulkanEngine::SceneObject::SceneObject()
    : transform(Eigen::Matrix4f::Identity()) {}

/// Update this scene object.
/// \param scene_state Contains information about the current state of the
/// scene.
void VulkanEngine::SceneObject::preUpdate(std::shared_ptr<SceneState> scene_state) {
  scene_state->setTransform(scene_state->getTotalTransform() * transform);
}

void VulkanEngine::SceneObject::update(std::shared_ptr<SceneState> scene_state) {
  /// Update all children
  for (const auto &child : children) {
    if (child.get() != nullptr) {
      child->preUpdate(scene_state);
      child->update(scene_state);
      child->postUpdate(scene_state);
    }
  }
}

/// Update this scene object.
/// \param scene_state Contains information about the current state of the
/// scene.
void VulkanEngine::SceneObject::postUpdate(std::shared_ptr<SceneState> scene_state) {
  scene_state->setTransform(scene_state->getTotalTransform() *
                           transform.inverse());
}

void VulkanEngine::SceneObject::addChildren(
    const std::vector<std::shared_ptr<SceneObject>> &_children) {
  children.insert(children.end(), _children.cbegin(), _children.cend());
}

const Eigen::Matrix4f VulkanEngine::SceneObject::getTransform() const {
  return transform;
}

void VulkanEngine::SceneObject::setTransform(
    const Eigen::Matrix4f &_transform) {
  transform = _transform;
}
