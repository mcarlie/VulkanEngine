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

#include <VulkanEngine/SceneObject.h>

#include <algorithm>
#include <memory>
#include <vector>

VulkanEngine::SceneObject::SceneObject()
    : transform(Eigen::Matrix4f::Identity()) {}

/// Update this scene object.
/// \param scene_state Contains information about the current state of the
/// scene.
void VulkanEngine::SceneObject::preUpdate(
    std::shared_ptr<SceneState> scene_state) {
  scene_state->setTransform(scene_state->getTotalTransform() * transform);
}

void VulkanEngine::SceneObject::update(
    std::shared_ptr<SceneState> scene_state) {
  /// Update all children
  for (const auto& child : children) {
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
void VulkanEngine::SceneObject::postUpdate(
    std::shared_ptr<SceneState> scene_state) {
  scene_state->setTransform(scene_state->getTotalTransform() *
                            transform.inverse());
}

void VulkanEngine::SceneObject::addChildren(
    const std::vector<std::shared_ptr<SceneObject>>& _children) {
  children.insert(children.end(), _children.cbegin(), _children.cend());
}

const Eigen::Matrix4f VulkanEngine::SceneObject::getTransform() const {
  return transform;
}

void VulkanEngine::SceneObject::setTransform(
    const Eigen::Matrix4f& _transform) {
  transform = _transform;
}
