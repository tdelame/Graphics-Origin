/*  Created on: Jan 7, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_BOX_H_
# define GRAPHICS_ORIGIN_BOX_H_
# include "../graphics_origin.h"
# include "traits.h"
# include "vec.h"

BEGIN_GO_NAMESPACE namespace geometry {
  class ball;
  class ray;
  class ray_with_inv_dir;

  /**@brief An axis aligned box.
   *
   * This class represents an axis aligned box by its center \c m_center
   * and its half sides \c m_half_sides. The upper and lower corners are then
   * \code{.cpp}
   * vec3 upper = m_center + m_half_sides;
   * vec3 lower = m_center - m_half_sides;
   * \endcode */
  struct aabox {
    /**@brief Create an empty box at the origin.
     *
     * Create an empty box at the origin.*/
    aabox();
    /**@brief Create a box with min and max values.
     *
     * Create a box that contains all points p with p[i] in [min[i],max[i]).
     * @param min Lower corner of the box.
     * @param max Upper corner of the box. */
    aabox( const vec3& min, const vec3& max );
    /**@brief Create a copy of a box.
     *
     * Create a box that is the copy of another one.
     * @param other The box to copy. */
    aabox( const aabox& other );
    /**@brief Copy a box into an existing one.
     *
     * Copy a box into this one.
     * @param other The box to copy. */
    aabox&
    operator=( const aabox& other );
    /**@brief Merge this box with another one.
     *
     * Merge this box with another one. The result box will
     * contain the two initial boxes.
     * @param other The other box to merge with this box. */
    void
    merge( const aabox& other ) noexcept;

    /**@brief Test if this box contains a point.
     *
     * Test if this box contains a point.
     * @param p The point to test.
     * @return True if the point is in this box. */
    bool
    contain( const vec3& p ) const;
    /**@brief Test if this box intersects a ball.
     *
     * Test if this box intersects a ball.
     * @param b The ball to test.
     * @return True if the ball intersects this box. */
    bool
    intersect( const ball& b ) const;
    /**@brief Test if this box intersects a ray.
     *
     * Test if this box intersects a ray.
     * @param r The ray to test.
     * @param t The distance between the closest intersection point and the ray origin.
     * @return True if the ray intersects this box*/
    bool
    intersect( const ray& r, real& t ) const;
    /**@brief Test if this box intersects a ray.
     *
     * Test if this box intersects a ray with a precomputed inverse direction.
     * This test is faster than the one with the normal ray.
     * @param r The ray to test.
     * @param t The distance between the closest intersection point and the ray origin.
     * @return True if the ray intersects this box.*/
    bool
    intersect( const ray_with_inv_dir& r, real& t ) const;
    /**@brief Compute the bounding box of this box.
     *
     * Compute the bounding box of this box, which is the box itself. This function
     * seems useless, but it is kept since it is trivial to implement.
     * @param b The bounding box computed.*/
    void
    compute_bounding_box( aabox& b ) const;
    /**@brief Access to the lower corner.
     *
     * Get the lower corner of the box.
     * @return The lower corner of this box. */
    inline vec3 get_min() const
    {
      return m_center - m_hsides;
    }
    /**@brief Access to the uppder corner.
     *
     * Get the upper corner of this box.
     * @return The upper worner of this box.*/
    inline vec3 get_max() const
    {
      return m_center + m_hsides;
    }

    vec3 m_center;
    vec3 m_hsides;
  };

  template<>
  struct geometric_traits<aabox> {
    static const bool is_ball_intersecter = true;
    static const bool is_bounding_box_computer = true;
    static const bool is_bounding_volume_merger = true;
    static const bool is_point_container = true;
    static const bool is_ray_intersecter = true;
    static const bool is_ray_with_inversed_direction_intersecter = true;
  };
}
END_GO_NAMESPACE
# endif
