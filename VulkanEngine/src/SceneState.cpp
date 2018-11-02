#include <VulkanEngine/SceneState.h>
#include <VulkanEngine/Camera.h>

const Eigen::Matrix4f& VulkanEngine::SceneState::getTransform() {
  return total_transform;
}

void VulkanEngine::SceneState::setTransform( const Eigen::Matrix4f& transform ) {
  total_transform = transform;
}

const Eigen::Matrix4f& VulkanEngine::SceneState::getTotalTransform() {
  return total_transform;
}

const Eigen::Matrix4f& VulkanEngine::SceneState::getViewMatrix() {
  return view_matrix;
}

void VulkanEngine::SceneState::setViewMatrix( const Eigen::Matrix4f& _view_matrix ) {
  view_matrix = _view_matrix;
}

const Eigen::Matrix4f& VulkanEngine::SceneState::getProjectionMatrix() {
  return projection_matrix;
}

void VulkanEngine::SceneState::setProjectionMatrix( const Eigen::Matrix4f& _projection_matrix ) {
  projection_matrix = _projection_matrix;
}