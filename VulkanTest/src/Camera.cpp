#ifndef CAMERA_CPP
#define CAMERA_CPP

#include <VulkanTest/Camera.h>

#include <VulkanTest/Utils.h>

template< typename Scalar >
VulkanTest::Camera< Scalar >::Camera( 
  Eigen::Matrix< Scalar, 3, 1 > _position,
  Eigen::Matrix< Scalar, 3, 1 > _look_at,
  Eigen::Matrix< Scalar, 3, 1 > _up_vector,
  Scalar _z_near,
  Scalar _z_far,
  Scalar _fov,
  uint32_t _width,
  uint32_t _height ) :
  position( _position ),
  look_at( _look_at ),
  up_vector( _up_vector ),
  z_near( _z_near ),
  z_far( _z_far ),
  fov( _fov ),
  width( _width ),
  height( _height ) {

}

template< typename Scalar > 
VulkanTest::Camera< Scalar >::~Camera() {

}

template< typename Scalar > 
Eigen::Matrix< Scalar, 4, 4 > VulkanTest::Camera< Scalar >::getPerspectiveProjectionMatrix() {

  Scalar f = 1 / std::tan( fov * static_cast< Scalar >( PI ) / ( 180 * 2 ) );
  Scalar aspect = static_cast< Scalar >( width ) / static_cast< Scalar >( height );

  Eigen::Matrix< Scalar, 4, 4 > perspective_projection_matrix;
  perspective_projection_matrix <<
    f / aspect, 0, 0, 0,
    0, -f, 0, 0, 
    0, 0, z_far / ( z_near - z_far ), -1,
    0, 0, ( z_near * z_far ) / ( z_near - z_far ), 1;

  // Adjust to Vulkan coordinate system
  // https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
  Eigen::Matrix< Scalar, 4, 4 > correction_matrix;
  correction_matrix <<
    1, 0, 0, 0,
    0, -1, 0, 0,
    0, 0, 0.5, 0.5,
    0, 0, 0, 1;

  return correction_matrix * perspective_projection_matrix;

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