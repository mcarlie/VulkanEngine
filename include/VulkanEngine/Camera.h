#ifndef CAMERA_H
#define CAMERA_H

#include <VulkanEngine/SceneObject.h>

#include <Eigen/Eigen>
#include <vulkan/vulkan.hpp>

namespace VulkanEngine {

/// Represents a camera in the scene.
/// The camera provides view and projection matrices for rendering
/// based on the current state of the camera object.
class Camera : public SceneObject {
public:
  /// Constructor.
  /// \param _look_at What position the Camera should be oriented towards
  /// initially. \param _up_vector The up direction of the Camera. \param
  /// _z_near The distance from the Camera's position to the near plane. \param
  /// _z_far The distance from the Camera's position to the far plane. \param
  /// _fov The field of view of the Camera. \param _width The width of the
  /// camera. \param _height The height of the camera.
  Camera(Eigen::Vector3f _look_at = {0.0f, 0.0f, 0.0f},
         Eigen::Vector3f _up_vector = {0.0f, 1.0f, 0.0f}, float _z_near = 0.1f,
         float _z_far = 1000.0f, float _fov = 45.0f, uint32_t _width = 800,
         uint32_t _height = 600);

  /// Destructor.
  virtual ~Camera();

  /// Set the look at vector of the camera.
  /// \param _look_at The look at vector to use.
  void setLookAt(const Eigen::Vector3f &_look_at);

  /// \return The current look at vector
  const Eigen::Vector3f getLookAt() const;

  /// \return The current up vector.
  const Eigen::Vector3f getUpVector() const;

  /// \return The projection matrix of the Camera.
  const Eigen::Matrix4f getPerspectiveProjectionMatrix() const;

  /// \param _transform The transformation matrix to use when calculating the
  /// view matrix \return The view matrix of the Camera.
  const Eigen::Matrix4f getViewMatrix() const;

  /// Set the width of the camera.
  /// \param _width The width of the camera.
  void setWidth(uint32_t _width);

  /// Set the heigth of the camera.
  /// \param _height The height of the camera.
  void setHeight(uint32_t _height);

  /// \return The width of the camera.
  float getWidth() const;

  /// \return The height of the camera.
  float getHeight() const;

private:
  /// Update the camera. Updates the projection and view matrix in \c
  /// scene_state with the camera's values. \param scene_state Represents the
  /// current state of the scene.
  virtual void update(SceneState &scene_state) override;

  /// What position the Camera is oriented towards.
  Eigen::Vector3f look_at;

  /// The up direction of the Camera.
  Eigen::Vector3f up_vector;

  /// The distance from the Camera's position to the near plane.
  float z_near;

  /// The distance from the Camera's position to the far plane.
  float z_far;

  /// The field of view of the Camera.
  float fov;

  /// The current width of the Camera.
  uint32_t width;

  /// The current height of the Camera.
  uint32_t height;
};

} // namespace VulkanEngine

#endif /* CAMERA_H */
