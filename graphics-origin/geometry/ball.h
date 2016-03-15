/*  Created on: Sep 27, 2015
 *      Author: T.Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN__BALL_H_
# define GRAPHICS_ORIGIN__BALL_H_

# include "traits.h"
# include "vec.h"

BEGIN_GO_NAMESPACE
namespace geometry {

  class aabox;
  struct ball
    : public vec4 {

    /**Creates unit ball*/
    ball() noexcept;

    ball( const vec3& center, const real& radius ) noexcept;
    ball( const ball&& other ) noexcept;
    ball( const vec4& b ) noexcept;

    ball&
    operator=( const ball&& other ) noexcept;

    bool intersect( const ball& b ) const;
    void compute_bounding_box( aabox& b ) const;
    void compute_bounding_ball( ball& b ) const;
    bool contain( const aabox& b ) const;
    bool intersect( const aabox& b ) const;
    bool contain( const vec3& p ) const;
    void merge( const ball& other );
  };

  template<>
  struct geometric_traits<ball> {
    static const bool is_ball_intersecter = true;
    static const bool is_bounding_box_computer = true;
    static const bool is_bounding_ball_computer = true;
    static const bool is_bounding_volume_merger = true;
    static const bool is_box_container = true;
    static const bool is_box_intersecter = true;
    static const bool is_point_container = true;
  };

}
END_GO_NAMESPACE
# endif
