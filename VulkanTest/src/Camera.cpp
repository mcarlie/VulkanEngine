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

  const Scalar tan_half_fov = std::tan( static_cast< Scalar >( PI ) * fov / ( static_cast< Scalar >( 2 ) * static_cast< Scalar >( 180 ) ) );
  const Scalar aspect = width / static_cast< Scalar >( height );

	Eigen::Matrix< Scalar, 4, 4 > result;
  result.setZero();
  result( 0, 0 ) = 1 / ( aspect * tan_half_fov );
	result( 1, 1 ) = 1 / tan_half_fov;
	result( 2, 2 ) = z_far / ( z_near - z_far );
	result( 3, 2 ) = -1;
	result( 2, 3 ) = -( z_far * z_near ) / ( z_far - z_near );

  return result;

}

template< typename Scalar > 
Eigen::Matrix< Scalar, 4, 4 > VulkanTest::Camera< Scalar >::getViewMatrix() {

	const Eigen::Matrix< Scalar, 3, 1 > z_axis = ( look_at - position ).normalized();
	const Eigen::Matrix< Scalar, 3, 1 > x_axis = z_axis.cross( up_vector ).normalized();
	const Eigen::Matrix< Scalar, 3, 1 > y_axis = x_axis.cross( z_axis );

	Eigen::Matrix< Scalar, 4, 4 > result = Eigen::Matrix< Scalar, 4, 4 >::Identity();
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

#endif /* CAMERA_CPP */