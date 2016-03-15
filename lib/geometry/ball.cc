/*  Created on: Jan 24, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/geometry/ball.h"
# include "../../graphics-origin/geometry/box.h"
# include "../../graphics-origin/tools/log.h"
BEGIN_GO_NAMESPACE
namespace geometry {

  ball::ball() noexcept
    : vec4{}
  {}

  ball::ball( const vec3& center, const real& radius ) noexcept
    : vec4{ center, radius }
  {}

  ball::ball( const ball&& other ) noexcept
    : vec4{ other }
  {}

  ball::ball( const vec4& b ) noexcept
    : vec4{ b }
  {}

  ball&
  ball::operator=( const ball&& other ) noexcept
  {
    vec4::operator=( std::move( other ) );
    return *this;
  }

  bool ball::intersect( const ball& b ) const
  {
    auto diff = vec4{ b.x - x, b.y - y, b.z - z, b.w + b.w };
    return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z < diff.w * diff.w;
  }

  void ball::compute_bounding_box( aabox& b ) const
  {
    b = aabox{ vec3{*this}, vec3{w,w,w} };
  }

  bool ball::intersect( const aabox& b ) const
  {
    auto ball_interiority = w * w;
    auto diff = glm::abs( vec3{*this} - b.get_center()) - b.get_half_sides();
    for( int i = 0; i < 3; ++ i )
      if( diff[i] > 0 )
        {
          ball_interiority -= diff[i] * diff[i];
        }
    return ball_interiority >= 0;
  }

  bool ball::contain( const aabox& b ) const
  {
    LOG( debug, "TODO");
    return false;
  }

  void ball::compute_bounding_ball( ball& b ) const
  {
    b = std::move(*this);
  }

  void ball::merge( const ball& other )
  {
    LOG( debug, "TODO");
  }

  bool ball::contain( const vec3& p ) const
  {
    auto diff = vec3{ p.x - x, p.y - y, p.z };
    return dot( diff, diff ) <= w * w;
  }
}
END_GO_NAMESPACE
