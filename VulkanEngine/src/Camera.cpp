#ifndef CAMERA_CPP
#define CAMERA_CPP

#include <VulkanEngine/Camera.h>
#include <VulkanEngine/Constants.h>
#include <VulkanEngine/Scene.h>

VulkanEngine::Camera::Camera( 
  Eigen::Vector3f _look_at,
  Eigen::Vector3f _up_vector,
  float _z_near,
  float _z_far,
  float _fov,
  uint32_t _width,
  uint32_t _height ) :
  look_at( _look_at ),
  up_vector( _up_vector ),
  z_near( _z_near ),
  z_far( _z_far ),
  fov( _fov ),
  width( _width ),
  height( _height ) {

}

VulkanEngine::Camera::~Camera() {

}

void VulkanEngine::Camera::update( SceneState& scene_state ) {
  
  auto active_window = scene_state.getScene().getActiveWindow();
  setWidth( active_window->getFramebufferWidth() );
  setHeight( active_window->getFramebufferHeight() );

  scene_state.setViewMatrix( scene_state.getTotalTransform() * getTransform().inverse() * getViewMatrix() );
  scene_state.setProjectionMatrix( getPerspectiveProjectionMatrix() );
  
  SceneObject::update( scene_state );

}

void VulkanEngine::Camera::setLookAt( const Eigen::Vector3f& _look_at ) {
  look_at = _look_at;
}

const Eigen::Vector3f& VulkanEngine::Camera::getLookAt() {
  return look_at;
}

const Eigen::Vector3f& VulkanEngine::Camera::getUpVector() {
  return up_vector;
}

const Eigen::Matrix4f VulkanEngine::Camera::getPerspectiveProjectionMatrix() {

  const float tan_half_fov = std::tan( Constants::pi< float >() * fov / ( 2.0f * 180.0f ) );
  const float aspect = width / static_cast< float >( height );

	Eigen::Matrix4f result;
  result.setZero();
  result( 0, 0 ) = 1.0f / ( aspect * tan_half_fov );
	result( 1, 1 ) = 1.0f / tan_half_fov;
	result( 2, 2 ) = z_far / ( z_near - z_far );
	result( 3, 2 ) = -1.0f;
	result( 2, 3 ) = -( z_far * z_near ) / ( z_far - z_near );

  Eigen::Matrix4f clip;
  clip.setZero();
  clip( 0, 0 ) = 1.0f;
  clip( 1, 1 ) = -1.0f;
  clip( 2, 2 ) = 0.5f;
  clip( 2, 3 ) = 0.5f;
  clip( 3, 3 ) = 1.0f;

  return clip * result;

}

const Eigen::Matrix4f VulkanEngine::Camera::getViewMatrix() {

  Eigen::Vector3f position = Eigen::Affine3f( getTransform() ).translation();
  
	const auto& z_axis = ( look_at - position ).normalized();
	const auto& x_axis = z_axis.cross( up_vector ).normalized();
	const auto& y_axis = x_axis.cross( z_axis );

	Eigen::Matrix4f result = Eigen::Matrix4f::Identity();
	result( 0, 0 ) = x_axis( 0 );
	result( 0, 1 ) = x_axis( 1 );
	result( 0, 2 ) = x_axis( 2 );
	result( 1, 0 ) = y_axis( 0 );
	result( 1, 1 ) = y_axis( 1 );
	result( 1, 2 ) = y_axis( 2 );
	result( 2, 0 ) = -z_axis( 0 );
	result( 2, 1 ) = -z_axis( 1 );
	result( 2, 2 ) = -z_axis( 2 );
	result( 0, 3 ) = -x_axis.dot( position );
	result( 1, 3 ) = -y_axis.dot( position );
	result( 2, 3 ) = z_axis.dot( position );

  return result;

}

void VulkanEngine::Camera::setWidth( uint32_t _width ) {
  width = _width;
}

void VulkanEngine::Camera::setHeight( uint32_t _height ) {
  height = _height;
}

#endif /* CAMERA_CPP */
