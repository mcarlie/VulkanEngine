#ifndef CAMERA_CPP
#define CAMERA_CPP

#include <VulkanTest/Camera.h>

template< typename Scalar >
VulkanTest::Camera< Scalar >::Camera( 
  Eigen::Matrix< Scalar, 3, 1 > _position,
  Eigen::Matrix< Scalar, 3, 1 > _look_at,
  Eigen::Matrix< Scalar, 3, 1 > _up_vector,
  Scalar _z_near,
  Scalar _z_far,
  Scalar _aspect_ratio,
  Scalar _fov ) :
  position( _position ),
  look_at( _look_at ),
  up_vector( _up_vector ),
  z_near( _z_near ),
  z_far( _z_far ),
  aspect_ratio( _aspect_ratio ),
  fov( _fov ) {

}

template< typename Scalar > 
VulkanTest::Camera< Scalar >::~Camera() {

}

template< typename Scalar > 
Eigen::Matrix< Scalar, 4, 4 > VulkanTest::Camera< Scalar >::getProjectionMatrix() {

  Scalar scale_y = 1 / std::tan( fov / 2 ); 
  Scalar scale_x = scale_y / aspect_ratio; 
  Scalar denom = z_far - z_near;

  Eigen::Matrix< Scalar, 4, 4 > projection_matrix;
  projection_matrix << 
    scale_x, 0, 0, 0,
    0, scale_y, 0, 0,
    0, 0,-( z_far + z_near ) / denom, -1,
    0, 0, -2 * z_near * z_far / denom, 0;

  return projection_matrix;

}

template< typename Scalar > 
Eigen::Matrix< Scalar, 4, 4 > VulkanTest::Camera< Scalar >::getViewMatrix() {

  const Eigen::Matrix< Scalar, 3, 1 >& z_axis = ( position - look_at ).normalized();
  const Eigen::Matrix< Scalar, 3, 1 >& x_axis = up_vector.cross( z_axis ).normalized();
  const Eigen::Matrix< Scalar, 3, 1 >& y_axis = z_axis.cross( x_axis );

  Eigen::Matrix< Scalar, 4, 4 > view_matrix;
  view_matrix <<
      x_axis( 0 ), y_axis( 0 ), z_axis( 0 ), 0,
      x_axis( 1 ), y_axis( 1 ), z_axis( 1 ), 0,
      x_axis( 2 ), y_axis( 2 ), z_axis( 2 ), 0,
      -x_axis.dot( position ), -y_axis.dot( position ), -z_axis.dot( position ), 1;

  return view_matrix;

}

#endif /* CAMERA_CPP */