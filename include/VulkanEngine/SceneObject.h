// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INCLUDE_VULKANENGINE_SCENEOBJECT_H_
#define INCLUDE_VULKANENGINE_SCENEOBJECT_H_

#include <VulkanEngine/SceneState.h>

#include <memory>
#include <vector>

namespace VulkanEngine {

/// Represents an object in a scene.
class SceneObject {
 public:
  /// Contructor.
  SceneObject();

  /// Add child SceneObject instances which will inherit the transformation of
  /// this SceneObject. \param _children The children to add.
  void addChildren(const std::vector<std::shared_ptr<SceneObject>>& _children);

  /// Get the current local transformation matrix.
  /// \return The current local transformation matrix
  const Eigen::Matrix4f getTransform() const;

  /// Set the current local transformation matrix.
  /// \param _transform The desired local transformation matrix.
  void setTransform(const Eigen::Matrix4f& _transform);

 protected:
  /// Update this scene object.
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void preUpdate(std::shared_ptr<SceneState> scene_state);

  /// Update this scene object.
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void update(std::shared_ptr<SceneState> scene_state);

  /// Update this scene object.
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void postUpdate(std::shared_ptr<SceneState> scene_state);

  /// The transformation matrix of this SceneObject.
  /// By default it is set to identity.
  Eigen::Matrix4f transform;

 private:
  /// Contains any children of this scene object.
  std::vector<std::shared_ptr<SceneObject>> children;
};

}  // namespace VulkanEngine

#endif  // INCLUDE_VULKANENGINE_SCENEOBJECT_H_
