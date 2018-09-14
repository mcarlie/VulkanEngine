#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Eigen>

namespace VulkanTest {

  /// Represents a camera or viewpoint in the scene
  /// \tparam Scalar The scalar type to use
  template< typename Scalar >
  class Camera {

  public:

    /// Constructor
    /// \param _position The position of the camera
    /// \param _look_at What position the camera should be oriented towards initially
    /// \param _up_vector The up direction of the camera
    /// \param _z_near The distance from the camera's position to the near plane
    /// \param _z_far The distance from the camera's position to the far plane
    /// \param _aspect_ratio The aspect ratio of the camera
    /// \param _fov The field of view of the camera
    Camera( 
      Eigen::Matrix< Scalar, 3, 1 > _position = { 0, 0, 1 },
      Eigen::Matrix< Scalar, 3, 1 > _look_at = { 0, 0, 0 },
      Eigen::Matrix< Scalar, 3, 1 > _up_vector = { 0, 1, 0 },
      Scalar _z_near = 0.001,
      Scalar _z_far = 1000,
      Scalar _aspect_ratio = 1,
      Scalar _fov = 60 );

    /// Destructor
    ~Camera();

    /// \return The projection matrix of the camera
    Eigen::Matrix< Scalar, 4, 4 > getProjectionMatrix();

    /// \return The view matrix of the camera
    Eigen::Matrix< Scalar, 4, 4 > getViewMatrix();

  private:

    /// The position of the camera
    Eigen::Matrix< Scalar, 3, 1 > position;

    /// What position the camera the camera is oriented towards
    Eigen::Matrix< Scalar, 3, 1 > look_at;

    /// The up direction of the camera
    Eigen::Matrix< Scalar, 3, 1 > up_vector;

    /// The distance from the camera's position to the near plane
    Scalar z_near;

    /// The distance from the camera's position to the far plane
    Scalar z_far;

    /// The aspect ratio of the camera
    Scalar aspect_ratio;

    /// The field of view of the camera
    Scalar fov;

  };

}

#include <Camera.cpp>

#endif /* CAMERA_H */