// Copyright (c) 2025 Michael Carlie
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <VulkanEngine/Camera.h>
#include <VulkanEngine/Scene.h>
#include <VulkanEngine/SceneState.h>

VulkanEngine::SceneState::SceneState(const Scene& _scene)
    : scene(_scene),
      total_transform(Eigen::Matrix4f::Identity()),
      view_matrix(),
      projection_matrix() {}

const VulkanEngine::Scene& VulkanEngine::SceneState::getScene() const {
  return scene;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getTransform() const {
  return total_transform;
}

void VulkanEngine::SceneState::setTransform(const Eigen::Matrix4f& transform) {
  total_transform = transform;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getTotalTransform() const {
  return total_transform;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getViewMatrix() const {
  return view_matrix;
}

void VulkanEngine::SceneState::setViewMatrix(
    const Eigen::Matrix4f& _view_matrix) {
  view_matrix = _view_matrix;
}

const Eigen::Matrix4f VulkanEngine::SceneState::getProjectionMatrix() const {
  return projection_matrix;
}

void VulkanEngine::SceneState::setProjectionMatrix(
    const Eigen::Matrix4f& _projection_matrix) {
  projection_matrix = _projection_matrix;
}
