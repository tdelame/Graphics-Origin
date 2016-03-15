/*  Created on: Jan 7, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_BOX_H_
# define GRAPHICS_ORIGIN_BOX_H_
# include "../graphics_origin.h"
# include "traits.h"
# include "vec.h"

BEGIN_GO_NAMESPACE
namespace geometry {
class ball;
class aabox {
public:
  virtual
  ~aabox() = default;

  /**Create an empty boxes at the origin.*/
  aabox();
  aabox( const vec3& center, const vec3& half_sides );

  aabox( const aabox& other );
  aabox( aabox&& other );

  aabox&
  operator=( const aabox& other );



  const vec3&
  get_center() const;

  const vec3&
  get_half_sides() const;


  void
  set_center( const vec3& center );

  void
  set_half_sides( const vec3& hsides );

  vec3
  get_min() const;
  vec3
  get_max() const;

  /** Merge the two boxes into this one.
   * This box will be the minimum axis aligned bounding box containing the
   * two boxes.
   */
  void
  merge( const aabox& other ) noexcept;

  void
  merge( const vec3& point ) noexcept;

  bool
  contain( const vec3& p ) const;

  bool
  intersect( const ball& b ) const;

  void
  compute_bounding_box( aabox& b ) const;
private:
  vec3 m_center;
  vec3 m_half_sides;
};

  template<>
  struct geometric_traits<aabox> {
    static const bool is_ball_intersecter = true;
    static const bool is_bounding_box_computer = true;
    static const bool is_bounding_volume_merger = true;
    static const bool is_point_container = true;
  };

extern aabox
create_aabox_from_min_max( const vec3& min_point, const vec3& max_point );

}
END_GO_NAMESPACE
# endif
