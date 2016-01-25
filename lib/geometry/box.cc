/*  Created on: Jan 23, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/geometry/ball.h>
# include <graphics-origin/geometry/box.h>

BEGIN_GO_NAMESPACE
namespace geometry {

aabox::aabox()
  : center{}, half_sides{}
{}

aabox::aabox( const vec3& center, const vec3& half_sides )
    : center{ center }, half_sides{ half_sides }
{}

aabox::aabox( const aabox& other )
    : center{ other.center }, half_sides{ other.half_sides }
{}

aabox&
aabox::operator=( const aabox& other )
{
  center = other.center;
  half_sides = other.half_sides;
  return *this;
}

aabox
create_aabox_from_min_max( const vec3& min_point, const vec3& max_point )
{
  auto center = ( max_point + min_point ) * 0.5;
  return aabox{ center, max_point - center };
}

void
aabox::merge( const aabox& other ) noexcept
{
  *this = create_aabox_from_min_max(
      min( get_min(), other.get_min()),
      max( get_max(), other.get_max() ) );
}

const vec3&
aabox::get_center() const
{
  return center;
}

const vec3&
aabox::get_half_sides() const
{
  return half_sides;
}

vec3
aabox::get_min() const
{
  return center - half_sides;
}

vec3
aabox::get_max() const
{
  return center + half_sides;
}

bool
aabox::do_contain( const vec3& p ) const
{
  auto diff = p - center;
  return std::abs (diff.x) <= half_sides.x
      && std::abs (diff.y) <= half_sides.y
      && std::abs (diff.z) <= half_sides.z;
}

bool
aabox::do_intersect( const ball& b ) const
{
  auto ball_interiority = b.get_radius() * b.get_radius();
  auto diff = glm::abs(center - b.get_center()) - half_sides;
  for( int i = 0; i < 3; ++ i )
    if( diff[i] > 0 )
      {
        ball_interiority -= diff[i] * diff[i];
      }
  return ball_interiority >= 0;
}

void
aabox::do_compute_bounding_box( aabox& b) const
{
  b = *this;
}

}
END_GO_NAMESPACE
