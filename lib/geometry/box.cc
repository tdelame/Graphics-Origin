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
  : m_center{}, m_hsides{}
{}

aabox::aabox( const vec3& min, const vec3& max )
    : m_center{ real(0.5) * (min + max )  }, m_hsides{ max - m_center }
{}

aabox::aabox( const aabox& other )
    : m_center{ other.m_center }, m_hsides{ other.m_hsides }
{}

aabox&
aabox::operator=( const aabox& other )
{
  m_center = other.m_center;
  m_hsides = other.m_hsides;
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
  auto diff = p - m_center;
  return std::abs (diff.x) <= m_hsides.x
      && std::abs (diff.y) <= m_hsides.y
      && std::abs (diff.z) <= m_hsides.z;
}

bool
aabox::intersect( const ball& b ) const
{
  auto ball_interiority = b.w * b.w;
  auto diff = glm::abs(m_center - vec3{b}) - m_hsides;
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
  real t1 = m_center.x - r.m_origin.x;
  real t2 = (t1 + m_hsides.x) * r.m_inv_direction.x;
       t1 = (t1 - m_hsides.x) * r.m_inv_direction.x;

  real tmin = std::min( t1, t2 );
  real tmax = std::max( t1, t2 );

  t1 = m_center.y - r.m_origin.y;
  t2 = ( t1 + m_hsides.y ) * r.m_inv_direction.y;
  t1 = ( t1 - m_hsides.y ) * r.m_inv_direction.y;

  tmin = std::max( tmin, std::min( t1, t2 ) );
  tmax = std::min( tmax, std::max( t1, t2 ) );

  t1 = m_center.z - r.m_origin.z;
  t2 = ( t1 + m_hsides.z ) * r.m_inv_direction.z;
  t1 = ( t1 - m_hsides.z ) * r.m_inv_direction.z;

  t    = std::max( tmin, std::min( t1, t2 ) );
  tmax = std::min( tmax, std::max( t1, t2 ) );

  return tmax >= std::max( real(0), t );
}
}
END_GO_NAMESPACE
