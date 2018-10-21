#ifndef UTILITIES_H
#define UTILITIES_H

#include <VulkanEngine/Constants.h>

#include <Eigen/Eigen>

namespace VulkanEngine {

  /// Contains various utility classes, type definitions and functions.
  namespace Utilities {
  
    /// Convert degrees to radian
    template< typename T >
    T toRadians( const T& val ) { return val * Constants::pi< T >() / static_cast< T >( 180 ); }

    /// Helper function for combining the hash of one type with another.
    /// \tparam T The type to hash.
    /// \param seed The hash to combine with.
    /// \param v The value to hash.
    template< class T >
    inline void hashCombine( size_t& seed, const T& v ) {
      seed ^= std::hash< T >()( v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
    }

    /// Add std::hash definition for Eigen::Matrix.
    template< typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols >
    struct std::hash< Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols > > {
      typedef Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols > T;
      size_t operator()( T const& matrix ) const {
        size_t seed = 0;
        for( int i = 0; i < matrix.size(); ++i ) {
          auto elem = *( matrix.data() + i );
          VulkanEngine::Utilities::hashCombine< typename T::Scalar >( seed, elem );
        }
        return seed;
      }  
    };

    /// Allows iterating over a tuple using a visitor function pointer.
    /// https://gist.github.com/jks-liu/738976c06a0fbd547a64
    template< std::size_t i = 0, class C, class... Types >
    typename std::enable_if< i >= sizeof ... ( Types ), typename C >::type
	    tupleForEach( const std::tuple< Types ... >& t, C& op ) {
	    return op;
    }

    /// Allows iterating over a tuple using a visitor function pointer.
    /// https://gist.github.com/jks-liu/738976c06a0fbd547a64
    template< size_t i = 0, class C, class... Types >
    typename std::enable_if< i < sizeof ... ( Types ), typename C >::type
	  tupleForEach( const std::tuple< Types ... >& t, C& op ) {
      op( std::get < i >( t ) );
      return tupleForEach< i + 1, C, Types... >( t, op );
    }

    /// Hash a tuple by combining the hashes of each element.
    /// \tparam TupleArgs The template arguments passed to the tuple definition.
    template< class ... TupleArgs >
    struct std::hash< std::tuple< TupleArgs ... > > {
      size_t operator()( const std::tuple< TupleArgs... >& tuple_value ) const {
        size_t seed = 0;
        VulkanEngine::Utilities::tupleForEach( tuple_value, [ &seed ]( const auto& element ) {
          VulkanEngine::Utilities::hashCombine( seed, element );
        } );
        return seed;
      }
    };
  
  }

}

#endif /* UTILITIES_H */