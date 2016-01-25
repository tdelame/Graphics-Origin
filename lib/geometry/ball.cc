/*  Created on: Jan 24, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/geometry/ball.h>
# include <graphics-origin/geometry/box.h>
BEGIN_GO_NAMESPACE
namespace geometry {

  ball::ball() noexcept
    : center{}, radius{1.0}
  {}

  ball::ball( const vec3& center, const real& radius ) noexcept
    : center{ center }, radius{ radius }
  {}

  ball::ball( const ball&& other ) noexcept
    : center{ other.center }, radius{ other.radius }
  {}

  ball&
  ball::operator=( const ball&& other ) noexcept
  {
    center = other.center;
    radius = other.radius;
    return *this;
  }

  const vec3&
  ball::get_center() const noexcept
  {
    return center;
  }

  const real&
  ball::get_radius() const noexcept
  {
    return radius;
  }

  void
  ball::set_center( const vec3& p ) noexcept
  {
    center = p;
  }

  void
  ball::set_radius( const real& r ) noexcept
  {
    radius = r;
  }

  bool
  ball::do_intersect( const aabox& b ) const
  {

    auto ball_interiority = radius * radius;
    auto diff = glm::abs(center - b.get_center()) - b.get_half_sides();
    for( int i = 0; i < 3; ++ i )
      if( diff[i] > 0 )
        {
          ball_interiority -= diff[i] * diff[i];
        }
    return ball_interiority >= 0;
  }

  void
  ball::do_compute_bounding_box( aabox& b ) const
  {
    b = aabox{ center, vec3{radius, radius, radius} };
  }

  bool
  ball::do_contain( const vec3& p ) const
  {
    return sdistance( p, center ) <= radius * radius;
  }

}
END_GO_NAMESPACE
