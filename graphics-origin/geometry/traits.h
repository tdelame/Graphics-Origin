/*  Created on: Mar 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_TRAITS_H_
# define GRAPHICS_ORIGIN_TRAITS_H_
# include "../graphics_origin.h"
BEGIN_GO_NAMESPACE namespace geometry {
  template< typename T >
  struct geometric_traits {
    /**@brief True if T is a ball intersected.
     *
     * This member is true if T is a ball intersecter, i.e. is able to compute
     * if it intersects with a ball. When this is the case, T has a method with
     * the following signature:
     * \code{.cpp}
     * bool intersect( const ball& b ) const;
     * \endcode */
    static const bool is_ball_intersecter      = false;
    /**@brief True if T is a bounding box computer.
     *
     * True if T is a bounding box computer, i.e. is able to compute
     * an axis aligned bounding box of its geometry. When this is the case, T
     * has a method with the following signature:
     * \code{.cpp}
     * void compute_bounding_box( aabox& b ) const;
     * \endcode */
    static const bool is_bounding_box_computer = false;
    /**@brief True if T is a box container.
     *
     * True if T is a box container, i.e. is able to say if it contains
     * completely an axis aligned bounding box. When this is the case, T
     * has a method with the following signature:
     * \code{.cpp}
     * void contain( const aabox& b ) const;
     * \endcode */
    static const bool is_box_container         = false;
    /**@brief True if T is a box intersecter.
     *
     * True if T is a box intersected, i.e. is able to say if it intersects
     * the volume of an axis aligned box. When this is the case, T has a
     * method with the following signature:
     * \code{.cpp}
     * void intersect( const aabox& b ) const;
     * \endcode */
    static const bool is_box_intersecter       = false;
    /**@ brief True if T is a point container.
     *
     * True if T is a point container, i.e. is able to say if it contains
     * a point. When this is the case, T has a method with the following
     * signature:
     * \code{.cpp}
     * void contain( const vec3& p ) const;
     * \endcode */
    static const bool is_point_container       = false;
    /**@brief True if T is a ray intersected.
     *
     * True if T is a ray intersected, i.e. is able to say if it intersects
     * a ray. When this is the case, T has a method with the following signature:
     * \code{.cpp}
     * bool intersect( const ray& r, real& t ) const;
     * \endcode */
    static const bool is_ray_intersected       = false;
  };
} END_GO_NAMESPACE
# endif
