/*  Created on: Sep 27, 2015
 *      Author: T.Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN__BALL_H_
# define GRAPHICS_ORIGIN__BALL_H_

# include "../graphics_origin.h"
# include "./concepts/bounding_box_computer.h"
# include "./concepts/box_intersecter.h"
# include "./concepts/point_container.h"

BEGIN_GO_NAMESPACE
namespace geometry {

class ball :
    public box_intersecter,
    public bounding_box_computer,
    public point_container {
public:
  /**Creates unit ball*/
  ball() noexcept;
  ball( const vec3& center, const real& radius ) noexcept;
  ball( const ball&& other ) noexcept;

  ball&
  operator=( const ball&& other ) noexcept;

  const vec3&
  get_center() const noexcept;

  const real&
  get_radius() const noexcept;

  void
  set_center( const vec3& p ) noexcept;

  void
  set_radius( const real& r ) noexcept;

protected:

  bool
  do_intersect( const aabox& b ) const override;

  void
  do_compute_bounding_box( aabox& b ) const override;

  bool
  do_contain( const vec3& p ) const override;

  vec3 center;
  real radius;
};

}
END_GO_NAMESPACE
# endif
