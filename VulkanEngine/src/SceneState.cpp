#include <VulkanEngine/Camera.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/SceneState.h>

VulkanEngine::SceneState::SceneState(const Scene &_scene)
    : scene(_scene), total_transform(Eigen::Matrix4f::Identity()),
      view_matrix(), projection_matrix() {}

const VulkanEngine::Scene &VulkanEngine::SceneState::getScene() const {
  return scene;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getTransform() const {
  return total_transform;
}

void VulkanEngine::SceneState::setTransform(const Eigen::Matrix4f &transform) {
  total_transform = transform;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getTotalTransform() const {
  return total_transform;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getViewMatrix() const {
  return view_matrix;
}

void VulkanEngine::SceneState::setViewMatrix(
    const Eigen::Matrix4f &_view_matrix) {
  view_matrix = _view_matrix;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getProjectionMatrix() const {
  return projection_matrix;
}

void VulkanEngine::SceneState::setProjectionMatrix(
    const Eigen::Matrix4f &_projection_matrix) {
  projection_matrix = _projection_matrix;
}
