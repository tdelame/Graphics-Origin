/*  Created on: Jan 23, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/ball.h"
# include "../../graphics-origin/geometry/box.h"
# include "../../graphics-origin/geometry/ray.h"

# include <utility>
# include <algorithm>
BEGIN_GO_NAMESPACE
namespace geometry {

aabox::aabox()
  : center{}, hsides{}
{}

aabox::aabox( const vec3& min, const vec3& max )
    : center{ real(0.5) * (min + max )  }, hsides{ max - center }
{}

aabox::aabox( const aabox& other )
    : center{ other.center }, hsides{ other.hsides }
{}

aabox&
aabox::operator=( const aabox& other )
{
  center = other.center;
  hsides = other.hsides;
  return *this;
}

void
aabox::merge( const aabox& other ) noexcept
{
  *this = aabox(
      min( get_min(), other.get_min()),
      max( get_max(), other.get_max() ) );
}

bool
aabox::contain( const vec3& p ) const
{
  auto diff = p - center;
  return std::abs (diff.x) <= hsides.x
      && std::abs (diff.y) <= hsides.y
      && std::abs (diff.z) <= hsides.z;
}

bool
aabox::intersect( const ball& b ) const
{
  auto ball_interiority = b.w * b.w;
  auto diff = glm::abs(center - vec3{b}) - hsides;
  for( int i = 0; i < 3; ++ i )
    if( diff[i] > 0 )
      {
        ball_interiority -= diff[i] * diff[i];
      }
  return ball_interiority >= 0;
}

void
aabox::compute_bounding_box( aabox& b) const
{
  b = *this;
}

bool
aabox::intersect( const ray& r, real& t ) const
{
  return intersect( ray_with_inv_dir( r ), t );
}

bool
aabox::intersect( const ray_with_inv_dir& r, real& t ) const
{
  //note: If we represented the box with its lower and upper corners
  // this function would cost 3 addition/subtraction less than the
  // current version.
  real t1 = center.x - r.m_origin.x;
  real t2 = (t1 + hsides.x) * r.m_inv_direction.x;
       t1 = (t1 - hsides.x) * r.m_inv_direction.x;

  real tmin = std::min( t1, t2 );
  real tmax = std::max( t1, t2 );

  t1 = center.y - r.m_origin.y;
  t2 = ( t1 + hsides.y ) * r.m_inv_direction.y;
  t1 = ( t1 - hsides.y ) * r.m_inv_direction.y;

  tmin = std::max( tmin, std::min( t1, t2 ) );
  tmax = std::min( tmax, std::max( t1, t2 ) );

  t1 = center.z - r.m_origin.z;
  t2 = ( t1 + hsides.z ) * r.m_inv_direction.z;
  t1 = ( t1 - hsides.z ) * r.m_inv_direction.z;

  t    = std::max( real(0), std::max( tmin, std::min( t1, t2 ) ) );
  tmax = std::min( tmax, std::max( t1, t2 ) );

  return tmax >= std::max( real(0), t );
}
}
END_GO_NAMESPACE
