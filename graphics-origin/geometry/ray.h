/*  Created on: Mar 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_RAY_H_
# define GRAPHICS_ORIGIN_RAY_H_
# include "../graphics_origin.h"
# include "vec.h"

BEGIN_GO_NAMESPACE namespace geometry {

  /**@brief A class to represent a ray.
   *
   * A ray is like an half-line: it is a line that is infinite
   * in one direction and with a end point in the other direction. This
   * end point is named the origin of the ray. A ray is then the set of
   * 3D points p such that p = origin + t * direction, with direction the
   * direction (normalized, pointing in the direction where the line is
   * infinite) of the ray and t >= 0.
   */
  struct ray {
    /**@brief Build a ray.
     *
     * Build a new ray.
     * @param from Origin of the ray.
     * @param direction Direction of the ray (should be normalized).
     */
    ray( const vec3& from,
         const vec3& direction )
      : m_origin{ from },
        m_direction{ direction }
    {}

    vec3 get_point( real t ) const
    {
      return m_origin + t * m_direction;
    }

    const vec3& get_origin() const
    {
      return m_origin;
    }

    const vec3& get_direction() const
    {
      return m_direction;
    }

    vec3 m_origin;
    vec3 m_direction;
  };

  struct ray_with_inv_dir {
    ray_with_inv_dir( const ray& r )
      : m_origin{ r.m_origin },
        m_inv_direction{
          real(1.0 / r.m_direction.x),
          real(1.0 / r.m_direction.y),
          real(1.0 / r.m_direction.z) }
    {}


    vec3 m_origin;
    vec3 m_inv_direction;
  };

} END_GO_NAMESPACE
# endif
