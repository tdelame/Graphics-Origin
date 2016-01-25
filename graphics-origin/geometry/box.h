/*  Created on: Jan 7, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_BOX_H_
# define GRAPHICS_ORIGIN_BOX_H_
# include "../graphics_origin.h"
# include "./concepts/ball_intersecter.h"
# include "./concepts/bounding_box_computer.h"
# include "./concepts/point_container.h"
# include "./vec.h"

BEGIN_GO_NAMESPACE
namespace geometry {

class aabox :
    public ball_intersecter,
    public bounding_box_computer,
    public point_container {
public:
  virtual
  ~aabox() = default;

  /**Create an empty boxes at the origin.*/
  aabox();
  aabox( const vec3& center, const vec3& half_sides );

  aabox( const aabox& other );

  aabox&
  operator=( const aabox& other );



  const vec3&
  get_center() const;

  const vec3&
  get_half_sides() const;

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


private:
  bool
  do_contain( const vec3& p ) const override;

  bool
  do_intersect( const ball& b ) const override;

  void
  do_compute_bounding_box( aabox& b ) const override;

  vec3 center;
  vec3 half_sides;
};

extern aabox
create_aabox_from_min_max( const vec3& min_point, const vec3& max_point );

}
END_GO_NAMESPACE
# endif
