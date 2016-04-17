/* Created on: Jan 25, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_TRIANGLE_H_
# define GRAPHICS_ORIGIN_TRIANGLE_H_

# include "../graphics_origin.h"
# include "vec.h"
# include "traits.h"

BEGIN_GO_NAMESPACE
namespace geometry {
  class ray;
  class aabox;
  class GO_API triangle {
  public:

    typedef enum{ V0, V1, V2 } vertex_index;

    triangle(
        const vec3& p1,
        const vec3& p2,
        const vec3& p3 );
    triangle( const triangle& t );
    triangle();

    triangle&
    operator=( const triangle& t );

    const vec3& get_vertex( vertex_index i ) const;
    const vec3& get_normal() const noexcept;


    void compute_bounding_box( aabox& b ) const;
    bool intersect( const aabox& b ) const;
    bool intersect( const ray& r, real& t ) const;

  private:
    vec3 vertices[3];
    vec3 normal;
  };

  template <>
  struct GO_API geometric_traits<triangle> {
    static const bool is_bounding_box_computer  = true;
    static const bool is_box_intersecter        = true;
    static const bool is_ray_intersecter        = true;
  };

}
END_GO_NAMESPACE
# endif 
