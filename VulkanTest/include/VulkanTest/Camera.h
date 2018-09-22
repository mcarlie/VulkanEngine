#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Eigen>
#include <vulkan/vulkan.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

namespace VulkanTest {

  /// Represents a camera or viewpoint in the scene
  /// \tparam Scalar The scalar type to use when calculating matrices
  template< typename Scalar >
  class Camera {

  public:

    /// Constructor
    /// \param _position The position of the Camera
    /// \param _look_at What position the Camera should be oriented towards initially
    /// \param _up_vector The up direction of the Camera
    /// \param _z_near The distance from the Camera's position to the near plane
    /// \param _z_far The distance from the Camera's position to the far plane
    /// \param _aspect_ratio The aspect ratio of the Camera
    /// \param _fov The field of view of the Camera
    Camera( 
      Eigen::Matrix< Scalar, 3, 1 > _position = { 10, 10, 10 },
      Eigen::Matrix< Scalar, 3, 1 > _look_at = { 0, 0, 0 },
      Eigen::Matrix< Scalar, 3, 1 > _up_vector = { 0, 0, 1 },
      Scalar _z_near = static_cast< Scalar >( 0.1 ),
      Scalar _z_far = 1000,
      Scalar _fov = 45,
      uint32_t _width = 800,
      uint32_t _height = 600 );

    /// Destructor
    ~Camera();

    /// \return The projection matrix of the Camera
    Eigen::Matrix< Scalar, 4, 4 > getPerspectiveProjectionMatrix();

    /// \return The view matrix of the Camera
    Eigen::Matrix< Scalar, 4, 4 > getViewMatrix();

  private:

    /// The position of the Camera
    Eigen::Matrix< Scalar, 3, 1 > position;

    /// What position the Camera is oriented towards
    Eigen::Matrix< Scalar, 3, 1 > look_at;

    /// The up direction of the Camera
    Eigen::Matrix< Scalar, 3, 1 > up_vector;

    /// The distance from the Camera's position to the near plane
    Scalar z_near;

    /// The distance from the Camera's position to the far plane
    Scalar z_far;

    /// The field of view of the Camera
    Scalar fov;

    /// The current width of the Camera
    uint32_t width;
    
    /// The current height of the Camera
    uint32_t height;

  };

}

#include <Camera.cpp>

#endif /* CAMERA_H */