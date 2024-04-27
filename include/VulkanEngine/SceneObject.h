#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

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
  void addChildren(const std::vector<std::shared_ptr<SceneObject>> &_children);

  /// Get the current local transformation matrix.
  /// \return The current local transformation matrix
  const Eigen::Matrix4f getTransform() const;

  /// Set the current local transformation matrix.
  /// \param _transform The desired local transformation matrix.
  void setTransform(const Eigen::Matrix4f &_transform);

protected:
  /// Update this scene object.
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void preUpdate(SceneState &scene_state);

  /// Update this scene object.
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void update(SceneState &scene_state);

  /// Update this scene object.
  /// \param scene_state Contains information about the current state of the
  /// scene.
  virtual void postUpdate(SceneState &scene_state);

  /// The transformation matrix of this SceneObject.
  /// By default it is set to identity.
  Eigen::Matrix4f transform;

private:
  /// Contains any children of this scene object.
  std::vector<std::shared_ptr<SceneObject>> children;
};

} // namespace VulkanEngine

#endif /* SCENEOBJECT_H */
