#ifndef SCENESTATE_H
#define SCENESTATE_H

#include <memory>
#include <Eigen/Eigen>

namespace VulkanEngine {

  /// Contains information about the current state of the scene.
  class SceneState {

  public:

    /// \return The current total transformation matrix
    const Eigen::Matrix4f& getTransform();

    /// Set the current total transformation matrix
    /// \param transform The transform to set
    void setTransform( const Eigen::Matrix4f& transform );

    /// \return The current total transform.
    const Eigen::Matrix4f& getTotalTransform();

    /// \return The current view matrix.
    const Eigen::Matrix4f& getViewMatrix();

    /// Set the current view matrix.
    void setViewMatrix( const Eigen::Matrix4f& _view_matrix );

    /// \return The current projection matrix.
    const Eigen::Matrix4f& getProjectionMatrix();

    /// Set the current projection matrix.
    void setProjectionMatrix( const Eigen::Matrix4f& _projection_matrix );

  private:

    /// The current total transformation matrix.
    Eigen::Matrix4f total_transform;

    /// The current view matrix.
    Eigen::Matrix4f view_matrix;

    /// The current projection matrix.
    Eigen::Matrix4f projection_matrix;

  };

}

#endif /* SCENESTATE_H */