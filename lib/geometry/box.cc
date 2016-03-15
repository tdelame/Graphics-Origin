/*  Created on: Jan 23, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/ball.h"
# include "../../graphics-origin/geometry/box.h"

# include <utility>
BEGIN_GO_NAMESPACE
namespace geometry {

aabox::aabox()
  : m_center{}, m_half_sides{}
{}

aabox::aabox( const vec3& center, const vec3& half_sides )
    : m_center{ center }, m_half_sides{ half_sides }
{}

aabox::aabox( const aabox& other )
    : m_center{ other.m_center }, m_half_sides{ other.m_half_sides }
{}

aabox::aabox( aabox&& other )
  : m_center{ std::move( other.m_center ) },
    m_half_sides{ std::move( other.m_half_sides ) }
{}

aabox&
aabox::operator=( const aabox& other )
{
  m_center = other.m_center;
  m_half_sides = other.m_half_sides;
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
  return m_center;
}

const vec3&
aabox::get_half_sides() const
{
  return m_half_sides;
}

vec3
aabox::get_min() const
{
  return m_center - m_half_sides;
}

vec3
aabox::get_max() const
{
  return m_center + m_half_sides;
}

bool
aabox::contain( const vec3& p ) const
{
  auto diff = p - m_center;
  return std::abs (diff.x) <= m_half_sides.x
      && std::abs (diff.y) <= m_half_sides.y
      && std::abs (diff.z) <= m_half_sides.z;
}

bool
aabox::intersect( const ball& b ) const
{
  auto ball_interiority = b.w * b.w;
  auto diff = glm::abs(m_center - vec3{b}) - m_half_sides;
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

void
aabox::set_center( const vec3& center )
{
  m_center = center;
}

void
aabox::set_half_sides( const vec3& hsides )
{
  m_half_sides = hsides;
}

}
END_GO_NAMESPACE
