// Copyright (c) 2024 Michael Carlie. All Rights Reserved.
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef SCENESTATE_H
#define SCENESTATE_H

#include <Eigen/Eigen>
#include <memory>

namespace VulkanEngine {

class Scene;

/// Contains information about the current state of the scene.
class SceneState {
 public:
  /// Constructor.
  SceneState(const Scene &_scene);

  const Scene &getScene() const;

  /// \return The current total transformation matrix.
  const Eigen::Matrix4f getTransform() const;

  /// Set the current total transformation matrix.
  /// \param transform The transform to set
  void setTransform(const Eigen::Matrix4f &transform);

  /// \return The current total transform.
  const Eigen::Matrix4f getTotalTransform() const;

  /// \return The current view matrix.
  const Eigen::Matrix4f getViewMatrix() const;

  /// Set the current view matrix.
  void setViewMatrix(const Eigen::Matrix4f &_view_matrix);

  /// \return The current projection matrix.
  const Eigen::Matrix4f getProjectionMatrix() const;

  /// Set the current projection matrix.
  void setProjectionMatrix(const Eigen::Matrix4f &_projection_matrix);

 private:
  const Scene &scene;

  /// The current total transformation matrix.
  Eigen::Matrix4f total_transform;

  /// The current view matrix.
  Eigen::Matrix4f view_matrix;

  /// The current projection matrix.
  Eigen::Matrix4f projection_matrix;
};

}  // namespace VulkanEngine

#endif /* SCENESTATE_H */
